%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Memory management: WeakPtr: List
%%% see associated .ch file for more info.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8
#include "../../rts.h"
%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Dummy values to fool linker into thinking file contains something
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8.dummyForLinker
int dummy_weakPtrList ;
%%]

%%[94 -8.dummyForLinker
%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% List internal defs
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[94
// initial size of weak ptr list
#define MM_WeakPtr_List_InitialSize 		100
%%]


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% WeakPtr List internal functions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[94
// get MM_WeakPtr_Object of mutator maintained/created object which embeds the MM_WeakPtr_Object
static inline MM_WeakPtr_Object* mm_weakPtr_List_WeakPtrOfObject( MM_WeakPtr_List_Data* weakPtrList, BPtr ptr ) {
	return (MM_WeakPtr_Object*)( ptr + weakPtrList->mutator->trace->objectHeaderNrBytes ) ;
}
%%]

static inline void mm_weakPtr_List_AddWeakPtrObjectToList( MM_FreeListArray* list, Word obj ) {
	MM_FlexArray_Inx inx = mm_freeListArray_AllocSlot( list ) ;
	MM_WeakPtr_ObjectAdmin* admin = (MM_WeakPtr_ObjectAdmin*)mm_freeListArray_At( list, inx ) ;
	admin->obj = obj ;
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% WeakPtr List internal functions, but still externally visible (because of inlining)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% WeakPtr List interface
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[94
void mm_weakPtr_List_Init( MM_WeakPtr* weakPtr, MM_Mutator* mutator, MM_Collector* collector ) {
	MM_WeakPtr_List_Data* weakPtrList = mm_malloc_LOF.malloc( sizeof(MM_WeakPtr_List_Data) ) ;
	
	weakPtrList->mutator = mutator ;
	weakPtrList->collector = collector ;
	weakPtrList->ptrList = mm_freeListArray_New( mutator->malloc, NULL, sizeof(MM_WeakPtr_ObjectAdmin), MM_WeakPtr_List_InitialSize, 0, 0 ) ;
		
	weakPtr->data = (MM_WeakPtr_Data_Priv*)weakPtrList ;
}

Word mm_weakPtr_List_NewWeakPtr( MM_WeakPtr* weakPtr, Word key, Word val, MM_WeakPtr_Finalizer finalizer ) {
	IF_GB_TR_ON(3,{printf("mm_weakPtr_List_NewWeakPtr key=%x val=%x\n", key, val) ;}) ;
	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;
	
	MM_LclRoot_EnterGrp ;
	MM_LclRoot_EnterOne2(key,val) ;
	BPtr ptr = weakPtrList->mutator->allocWeakPtr( weakPtrList->mutator ) ;
	MM_LclRoot_LeaveGrp ;

	MM_WeakPtr_Object* w = mm_weakPtr_List_WeakPtrOfObject( weakPtrList, ptr ) ;
	w->key = key ;
	w->val = val ;
	w->finalizer = finalizer ;
	
	mm_freeListArray_Add( weakPtrList->ptrList, (BPtr)&ptr ) ;
	IF_GB_TR_ON(3,{mm_weakPtr_List_Dump(weakPtr) ;}) ;
	IF_GB_TR_ON(3,{printf("mm_weakPtr_List_NewWeakPtr key=%x val=%x ptr=%p w=%p\n", key, val, ptr, w) ;}) ;
	
	return (Word)ptr ;
}

void mm_weakPtr_List_FinalizeWeakPtr( MM_WeakPtr* weakPtr, Word ptr ) {
	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;

	MM_WeakPtr_Object* w = mm_weakPtr_List_WeakPtrOfObject( weakPtrList, (BPtr)ptr ) ;
	Word key = w->key ;
	Word val = w->val ;
	MM_WeakPtr_Finalizer finalizer = w->finalizer ;
	
	w->key = 0 ;
	w->val = 0 ;
	w->finalizer = NULL ;

	IF_GB_TR_ON(3,{printf("mm_weakPtr_List_FinalizeWeakPtr wptr=%x w=%p key=%x val=%x finalizer=%p\n",ptr,w,key,val,finalizer) ;}) ;
	if ( finalizer != NULL ) {
		(*finalizer)( key, val ) ;
	}
}

void mm_weakPtr_List_StartFindLiveObjects( MM_WeakPtr* weakPtr ) {
	IF_GB_TR_ON(3,{printf("mm_weakPtr_List_StartFindLiveObjects\n") ;}) ;
	IF_GB_TR_ON(3,{mm_weakPtr_List_Dump(weakPtr) ;}) ;

	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;
	
	weakPtrList->ptrListNew = mm_freeListArray_New( weakPtrList->mutator->malloc, NULL, sizeof(MM_WeakPtr_ObjectAdmin), mm_freeListArray_SizeUsed(weakPtrList->ptrList), 0, 0 ) ;
}

void mm_weakPtr_List_FindLiveObjects( MM_WeakPtr* weakPtr, MM_WeakPtr_NewAlive* newAlive ) {
	IF_GB_TR_ON(3,{printf("mm_weakPtr_List_FindLiveObjects BEF\n") ;}) ;
	IF_GB_TR_ON(3,{mm_weakPtr_List_Dump(weakPtr) ;}) ;

	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;
	
	MM_FreeListArray* list = weakPtrList->ptrList ;

	newAlive->alive = weakPtrList->ptrListNew ;
	newAlive->firstAliveInx = mm_freeListArray_SizeUsed( weakPtrList->ptrListNew ) ;
	
	MM_Iterator iter ;
	for ( mm_freeListArray_Iterator( list, &iter ) ; iter.hasData ; iter.step( &iter ) ) {
		MM_FlexArray_Inx i = iter.state ;
		IF_GB_TR_ON(3,{printf("mm_weakPtr_List_FindLiveObjects list=%p i=%x admin=%p\n",list,i,iter.data) ;}) ;
		MM_WeakPtr_ObjectAdmin* admin = (MM_WeakPtr_ObjectAdmin*)iter.data ;
		// todo: indirection
		IF_GB_TR_ON(3,{printf("mm_weakPtr_List_FindLiveObjects A obj=%x\n",admin->obj) ;}) ;
		Word obj = weakPtrList->mutator->trace->ensureNoIndirections( weakPtrList->mutator->trace, admin->obj ) ;
		IF_GB_TR_ON(3,{printf("mm_weakPtr_List_FindLiveObjects B obj=%x wkptr=%p\n",obj,mm_weakPtr_List_WeakPtrOfObject( weakPtrList, (BPtr)obj )) ;}) ;
		MM_WeakPtr_Object* w = mm_weakPtr_List_WeakPtrOfObject( weakPtrList, (BPtr)obj ) ;
		Word key = weakPtrList->mutator->trace->ensureNoIndirections( weakPtrList->mutator->trace, w->key ) ;
		IF_GB_TR_ON(3,{printf("mm_weakPtr_List_FindLiveObjects C key=%x alive=%d\n",key,!weakPtrList->collector->isInCollectedSpace( weakPtrList->collector, key )) ;}) ;
		if ( ! weakPtrList->collector->isInCollectedSpace( weakPtrList->collector, key ) ) {
			// todo: it may be an int as well !!
			mm_freeListArray_Add( weakPtrList->ptrListNew, (BPtr)admin ) ;
			mm_freeListArray_DeallocSlot( list, i ) ;
		}
	}

	newAlive->aftLastAliveInx = mm_freeListArray_SizeUsed( weakPtrList->ptrListNew ) ;
}

MM_FreeListArray* mm_weakPtr_List_EndFindLiveObjects( MM_WeakPtr* weakPtr ) {
	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;
	
	MM_FreeListArray* toFinalize = weakPtrList->ptrList ;
	weakPtrList->ptrList = weakPtrList->ptrListNew ;
	
	return toFinalize ;
}

Word mm_weakPtr_List_TraceWeakPtr( MM_WeakPtr* weakPtr, MM_Trace* trace, Word obj ) {
	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;
	
	obj = mm_Trace_TraceObject( trace, obj, MM_Trace_Flg_All ) ;
	MM_WeakPtr_Object* w = mm_weakPtr_List_WeakPtrOfObject( weakPtrList, (BPtr)obj ) ;
	w->key = mm_Trace_TraceObject( trace, w->key, MM_Trace_Flg_All ) ;
	w->val = mm_Trace_TraceObject( trace, w->val, MM_Trace_Flg_All ) ;
	
	return obj ;
}

%%]

void mm_weakPtr_List_zzz( MM_WeakPtr* weakPtr, ... ) {
	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;
	
}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% WeakPtr List interface object
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[94
MM_WeakPtr mm_weakPtr_List =
	{ NULL
	, &mm_weakPtr_List_Init
	, &mm_weakPtr_List_NewWeakPtr
	, &mm_weakPtr_List_FinalizeWeakPtr
	, &mm_weakPtr_List_FindLiveObjects
	, &mm_weakPtr_List_StartFindLiveObjects
	, &mm_weakPtr_List_EndFindLiveObjects
	, &mm_weakPtr_List_TraceWeakPtr
	// , &
	} ;
%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% WeakPtr List dump
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[94
#ifdef TRACE
void mm_weakPtr_List_Dump( MM_WeakPtr* weakPtr ) {
	MM_WeakPtr_List_Data* weakPtrList = (MM_WeakPtr_List_Data*)weakPtr->data ;

	printf( ">------------------------> MM_WeakPtr: List: weakPtr=%x weakPtrList=%x\n", weakPtr, weakPtrList ) ;

	mm_freeListArray_Dump( weakPtrList->ptrList ) ;

	printf( "<------------------------< MM_WeakPtr: List\n" ) ;

}
#endif
%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% WeakPtr List test
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[94
#ifdef TRACE
void mm_weakPtr_List_Test() {
}
#endif
%%]

