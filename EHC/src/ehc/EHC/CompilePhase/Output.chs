%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% EHC Compile output generation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Output generation, on stdout or file

%%[8 module {%{EH}EHC.CompilePhase.Output}
%%]

-- general imports
%%[8 import(qualified EH.Util.FastSeq as Seq)
%%]
%%[8 import(qualified Data.Map as Map)
%%]

%%[8 import({%{EH}EHC.Common})
%%]
%%[8 import({%{EH}EHC.CompileUnit})
%%]
%%[8 import({%{EH}EHC.CompileRun})
%%]

%%[8 import(qualified {%{EH}Config} as Cfg)
%%]

-- HI syntax and semantics
%%[20 import(qualified {%{EH}HI} as HI)
%%]

-- Core output
%%[(8 codegen) import({%{EH}Core},{%{EH}Core.Pretty})
%%]
%%[(8 codegen) import({%{EH}TyCore.Pretty})
%%]
-- Grin input and output
%%[(8 codegen grin) import({%{EH}GrinCode.Pretty})
%%]
-- Java output
%%[(8 codegen java) import({%{EH}Core.ToJava})
%%]

-- serialization
%%[20 import(qualified {%{EH}Base.Binary} as Bin, {%{EH}Base.Serialize})
%%]
-- for debugging only
%%[20 import({%{EH}Gam})
%%]

-- module admin
%%[20 import({%{EH}Module})
%%]


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Compile actions: output
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[(8 codegen) export(cpOutputTyCore)
cpOutputTyCore :: String -> HsName -> EHCompilePhase ()
cpOutputTyCore suff modNm
  =  do  {  cr <- get
         -- part 1: current .tycore
         ;  let  (ecu,crsi,opts,fp) = crBaseInfo modNm cr
                 mbTyCore = ecuMbTyCore ecu
                 cMod   = panicJust "cpOutputTyCore" mbTyCore
                 fpC = mkOutputFPath opts modNm fp suff
         ;  cpMsg modNm VerboseALot "Emit TyCore"
         ;  lift $ putPPFPath fpC (ppModule opts cMod) 100
         }
%%]

%%[(8 codegen) export(cpOutputCoreModule,cpOutputCore)
cpOutputCoreModule :: String -> String -> HsName -> CModule -> EHCompilePhase ()
cpOutputCoreModule nmsuff suff modNm cMod
  =  do  {  cr <- get
         ;  let  (ecu,crsi,opts,fp) = crBaseInfo modNm cr
                 fpC = mkOutputFPath opts modNm fp (suff ++ nmsuff) -- for now nmsuff after suff, but should be inside name
         ;  lift $ putPPFPath fpC (ppCModule opts cMod) 100
         }

cpOutputCore :: String -> String -> HsName -> EHCompilePhase ()
cpOutputCore nmsuff suff modNm
  =  do  {  cr <- get
         -- part 1: current .core
         ;  let  (ecu,_,_,_) = crBaseInfo modNm cr
                 mbCore = ecuMbCore ecu
                 cMod   = panicJust "cpOutputCore" mbCore
         ;  cpMsg modNm VerboseALot "Emit Core"
         ;  cpOutputCoreModule nmsuff suff modNm cMod
         }
%%]

%%[(8 codegen java) export(cpOutputJava)
cpOutputJava :: String -> HsName -> EHCompilePhase ()
cpOutputJava suff modNm
  =  do  {  cr <- get
         ;  let  (ecu,crsi,opts,fp) = crBaseInfo modNm cr
                 mbCore = ecuMbCore ecu
                 cMod   = panicJust "cpOutputJava" mbCore
                 (jBase,jPP) = cmodJavaSrc cMod
                 -- fpJ = fpathSetBase jBase fp    
                 fpJ    = mkOutputFPath opts (mkHNm jBase) fp suff
         ;  cpMsg modNm VerboseALot "Emit Java"
         ;  lift (putPPFPath fpJ jPP 100)
         }
%%]

%%[(8 codegen grin) export(cpOutputGrin)
cpOutputGrin :: String -> HsName -> EHCompilePhase ()
cpOutputGrin suff modNm
  =  do  {  cr <- get
         ;  let  (ecu,crsi,opts,fp) = crBaseInfo modNm cr
                 mbGrin = ecuMbGrin ecu
                 grin   = panicJust "cpOutputGrin" mbGrin
                 grinPP = ppGrModule grin
                 mkb x  = x ++ suff
                 fpG    = mkOutputFPath opts (mkHNm $ mkb $ show modNm) (fpathUpdBase mkb fp) "grin"
         ;  when (True) -- ehcOptFullProgAnalysis opts)
                 (do { cpMsg modNm VerboseALot "Emit Grin"
                     ; lift $ putPPFPath fpG grinPP 1000 --TODO ? getal
                     })
         }

%%]

%%[(8 codegen grin) export(cpOutputByteCodeC)
cpOutputByteCodeC :: String -> HsName -> EHCompilePhase ()
cpOutputByteCodeC suff modNm
  =  do  {  cr <- get
         ;  let  (ecu,_,opts,fp) = crBaseInfo modNm cr
                 bc       = panicJust "cpOutputByteCodeC" $ ecuMbBytecodeSem ecu
                 fpC      = mkOutputFPath opts modNm fp suff
         ;  cpMsg' modNm VerboseALot "Emit ByteCode C" Nothing fpC     -- '
%%[[99
         ;  cpRegisterFilesToRm [fpC]
%%]]
         ;  lift $ putPPFPath fpC bc 150
         }
%%]

%%[20 export(cpOutputHI)
cpOutputHI :: String -> HsName -> EHCompilePhase ()
cpOutputHI suff modNm
  =  do  {  cr <- get
         ;  let  (ecu,crsi,opts,fp) = crBaseInfo modNm cr
                 mmi    = panicJust "cpOutputHI.crsiModMp" $ Map.lookup modNm $ crsiModMp crsi
                 hiinfo = (ecuHIInfo ecu)
                               { HI.hiiExps                 = mmiExps       mmi
                               , HI.hiiHiddenExps           = mmiHiddenExps mmi
                               , HI.hiiHasMain              = ecuHasMain ecu
                               , HI.hiiTargetVariant        = ehcOptTargetVariant opts
                               , HI.hiiSrcTimeStamp         = Cfg.verTimestamp Cfg.version
                               , HI.hiiSrcSig               = Cfg.verSig Cfg.version
                               , HI.hiiSrcVersionMajor      = Cfg.verMajor Cfg.version
                               , HI.hiiSrcVersionMinor      = Cfg.verMinor Cfg.version
                               , HI.hiiSrcVersionMinorMinor = Cfg.verMinorMinor Cfg.version
                               , HI.hiiSrcVersionSvn        = Cfg.verSvnRevision Cfg.version
                               , HI.hiiCompileFlags         = optsDiscrRecompileRepr opts
                               }
                 fpH    = mkOutputFPath opts modNm fp suff
                 fnH    = fpathToStr fpH
         ;  cpMsg modNm VerboseALot "Emit HI"
         ;  hiExists <- lift $ doesFileExist fnH
         ;  when (hiExists)
                 (lift $ removeFile fnH)
         ;  when (ehcOptVerbosity opts >= VerboseALot)
                 (do { lift $ putPPLn (pp hiinfo)
                     })
         ;  lift $ putSerializeFile fnH hiinfo
         ;  now <- lift $ getClockTime
         ;  cpUpdCU modNm (ecuStoreHIInfoTime now)
         }

%%]


