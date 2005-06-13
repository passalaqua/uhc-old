let  g1  ::  ((Int,a) -> (Int,a)) -> Int
     g2  ::  ((a,Int) -> (a,Int)) -> Int
     id  =   \x ->  x
     f   =   \h ->  let  y1  = g1 h
                         y2  = g2 h
                    in   y2
in   f id

