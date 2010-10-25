{- ----------------------------------------------------------------------------------------
   what    : library Data.Bits, for Word variants
   expected: ok
   platform: word size dependent
   constraints: exclude-if-jscript
---------------------------------------------------------------------------------------- -}

module DataBitsWord1 where

import Data.Bits
import Data.Word

main :: IO ()
main
  = do putStrLn ("Word8:")
       putStrLn (show (bitSize (2 :: Word8)))
       putStrLn (show (2 .&. 1 :: Word8))
       putStrLn (show (2 .|. 1 :: Word8))
       putStrLn (show ((2 :: Word8) `shiftL` (1 :: Int)))
       putStrLn (show ((2 :: Word8) `shiftR` (1 :: Int)))
       putStrLn (show ((-2 :: Word8) `shiftL` (1 :: Int)))
       putStrLn (show ((-2 :: Word8) `shiftR` (1 :: Int)))
       putStrLn (show (bit 1 :: Word8))
       putStrLn (show (complement 2 :: Word8))
       putStrLn (show ((2 :: Word8) `testBit` (1 :: Int)))
       putStrLn (show ((2 :: Word8) `testBit` (2 :: Int)))
       putStrLn (show ((2 :: Word8) `complementBit` (1 :: Int)))
       putStrLn (show ((2 :: Word8) `complementBit` (2 :: Int)))
       putStrLn (show ((2 :: Word8) `rotateL` (2 :: Int)))
       putStrLn (show ((2 :: Word8) `rotateR` (2 :: Int)))

       putStrLn ("\nWord16:")
       putStrLn (show (bitSize (2 :: Word16)))
       putStrLn (show (2 .&. 1 :: Word16))
       putStrLn (show (2 .|. 1 :: Word16))
       putStrLn (show ((2 :: Word16) `shiftL` (1 :: Int)))
       putStrLn (show ((2 :: Word16) `shiftR` (1 :: Int)))
       putStrLn (show ((-2 :: Word16) `shiftL` (1 :: Int)))
       putStrLn (show ((-2 :: Word16) `shiftR` (1 :: Int)))
       putStrLn (show (bit 1 :: Word16))
       putStrLn (show (complement 2 :: Word16))
       putStrLn (show ((2 :: Word16) `testBit` (1 :: Int)))
       putStrLn (show ((2 :: Word16) `testBit` (2 :: Int)))
       putStrLn (show ((2 :: Word16) `complementBit` (1 :: Int)))
       putStrLn (show ((2 :: Word16) `complementBit` (2 :: Int)))
       putStrLn (show ((2 :: Word16) `rotateL` (2 :: Int)))
       putStrLn (show ((2 :: Word16) `rotateR` (2 :: Int)))

       putStrLn ("\nWord32:")
       putStrLn (show (bitSize (2 :: Word32)))
       putStrLn (show (2 .&. 1 :: Word32))
       putStrLn (show (2 .|. 1 :: Word32))
       putStrLn (show ((2 :: Word32) `shiftL` (1 :: Int)))
       putStrLn (show ((2 :: Word32) `shiftR` (1 :: Int)))
       putStrLn (show ((-2 :: Word32) `shiftL` (1 :: Int)))
       putStrLn (show ((-2 :: Word32) `shiftR` (1 :: Int)))
       putStrLn (show (bit 1 :: Word32))
       putStrLn (show (complement 2 :: Word32))
       putStrLn (show ((2 :: Word32) `testBit` (1 :: Int)))
       putStrLn (show ((2 :: Word32) `testBit` (2 :: Int)))
       putStrLn (show ((2 :: Word32) `complementBit` (1 :: Int)))
       putStrLn (show ((2 :: Word32) `complementBit` (2 :: Int)))
       putStrLn (show ((2 :: Word32) `rotateL` (2 :: Int)))
       putStrLn (show ((2 :: Word32) `rotateR` (2 :: Int)))

       putStrLn ("\nWord64:")
       putStrLn (show (bitSize (2 :: Word64)))
       putStrLn (show (2 .&. 1 :: Word64))
       putStrLn (show (2 .|. 1 :: Word64))
       putStrLn (show ((2 :: Word64) `shiftL` (1 :: Int)))
       putStrLn (show ((2 :: Word64) `shiftR` (1 :: Int)))
       putStrLn (show ((-2 :: Word64) `shiftL` (1 :: Int)))
       putStrLn (show ((-2 :: Word64) `shiftR` (1 :: Int)))
       putStrLn (show (bit 1 :: Word64))
       putStrLn (show (complement 2 :: Word64))
       putStrLn (show ((2 :: Word64) `testBit` (1 :: Int)))
       putStrLn (show ((2 :: Word64) `testBit` (2 :: Int)))
       putStrLn (show ((2 :: Word64) `complementBit` (1 :: Int)))
       putStrLn (show ((2 :: Word64) `complementBit` (2 :: Int)))
       putStrLn (show ((2 :: Word64) `rotateL` (2 :: Int)))
       putStrLn (show ((2 :: Word64) `rotateR` (2 :: Int)))

       putStrLn ("\nWord:")
       putStrLn (show (bitSize (2 :: Word)))
       putStrLn (show (2 .&. 1 :: Word))
       putStrLn (show (2 .|. 1 :: Word))
       putStrLn (show ((2 :: Word) `shiftL` (1 :: Int)))
       putStrLn (show ((2 :: Word) `shiftR` (1 :: Int)))
       putStrLn (show ((-2 :: Word) `shiftL` (1 :: Int)))
       putStrLn (show ((-2 :: Word) `shiftR` (1 :: Int)))
       putStrLn (show (bit 1 :: Word))
       putStrLn (show (complement 2 :: Word))
       putStrLn (show ((2 :: Word) `testBit` (1 :: Int)))
       putStrLn (show ((2 :: Word) `testBit` (2 :: Int)))
       putStrLn (show ((2 :: Word) `complementBit` (1 :: Int)))
       putStrLn (show ((2 :: Word) `complementBit` (2 :: Int)))
       putStrLn (show ((2 :: Word) `rotateL` (2 :: Int)))
       putStrLn (show ((2 :: Word) `rotateR` (2 :: Int)))

