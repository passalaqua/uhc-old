{-# LANGUAGE RankNTypes #-}

module TestLude where

infixr 1 $

data List a = Cons a (List a)
        | Nil
data Bool = True | False
data ST s a  = ST (State s -> (State s, a))
data State s = State s
data Ref s a = Ref s a
data Tup2 a b = Tup2 a b

foreign import ccall "Prelude" (+)     :: Int -> Int -> Int

-- builtin functions
foreign import ccall "Prelude" ($) :: forall a b. (a -> b) -> a -> b
foreign import ccall "Prelude" iff :: forall a. Bool -> a -> a -> a
 
-- standard functions
foreign import ccall "Prelude" id        :: forall a. a -> a
foreign import ccall "Prelude" apply     :: forall a b. (a -> b) -> a -> b
foreign import ccall "Prelude" const     :: forall a b. a -> b -> a
foreign import ccall "Prelude" choose    :: forall a. a -> a -> a
foreign import ccall "Prelude" revapp    :: forall a b. a -> (a -> b) -> b
foreign import ccall "Prelude" undefined :: forall a. a

--  integers
foreign import ccall "Prelude" plus :: Int -> Int -> Int
foreign import ccall "Prelude" lt   :: Int -> Int -> Bool
foreign import ccall "Prelude" gt   :: Int -> Int -> Bool
foreign import ccall "Prelude" inc  :: Int -> Int

-- polymorphic functions
foreign import ccall "Prelude" ids      :: List (forall a. a -> a)
foreign import ccall "Prelude" auto     :: (forall a. a -> a) -> (forall a. a -> a)
foreign import ccall "Prelude" xauto    :: forall a. (forall b. b -> b) -> a -> a
foreign import ccall "Prelude" takeAuto :: ((forall a. a -> a) -> (forall a. a -> a)) -> (forall a. a -> a)

-- lists
foreign import ccall "Prelude" single :: forall a. a -> List a
foreign import ccall "Prelude" head   :: forall a. List a -> a
foreign import ccall "Prelude" tail   :: forall a. List a -> List a
foreign import ccall "Prelude" map    :: forall a b. (a -> b) -> List a -> List b
foreign import ccall "Prelude" length :: forall a. List a -> Int
foreign import ccall "Prelude" null   :: forall a. List a-> Bool
foreign import ccall "Prelude" append :: forall a. List a -> List a -> List a

-- tuples
foreign import ccall "Prelude" fst :: forall a b. Tup2 a b -> a
foreign import ccall "Prelude" snd :: forall a b. Tup2 a b -> b

-- ST monad
foreign import ccall "Prelude" runST    :: forall a. (forall s. ST s a) -> a
foreign import ccall "Prelude" newRef   :: forall a s. a -> ST s (Ref s a)
foreign import ccall "Prelude" returnST :: forall a s. a -> ST s a
-- < 1

-- > 2
foreign import ccall "Prelude" foldr   :: forall a b. (a -> b -> b) -> b -> List a -> b
foreign import ccall "Prelude" foldl   :: forall a b. (a -> b -> a) -> a -> List b -> a
foreign import ccall "Prelude" zipWith :: forall a b c. (a -> b -> c) -> List a -> List b -> List c    
foreign import ccall "Prelude" zip     :: List a -> List b -> List (Tup2 a b)
foreign import ccall "Prelude" (!!)    :: List a -> Int -> a
foreign import ccall "Prelude" (&&)    :: Bool -> Bool -> Bool
foreign import ccall "Prelude" not     :: Bool -> Bool
foreign import ccall "Prelude" (++)    :: List a -> List a -> List a
foreign import ccall "Prelude" filter  :: (a -> Bool) -> List a -> List a
foreign import ccall "Prelude" all     :: (a -> Bool) -> List a -> Bool
foreign import ccall "Prelude" and     :: List Bool -> Bool 