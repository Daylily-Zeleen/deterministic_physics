﻿

#include <iostream>
#include "glacier_time.h"
#include "glacier_distance.h"
#include "glacier_collision_gjk.h"
#include "glacier_collision_sphere.h"


int main()
{

     GVector3 Point( f32(1),f32(1), f32(1) );
     GVector3 S0(f32(2), f32(1), f32(1));
     GVector3 S1(f32(4), f32(1), f32(1));
     GVector3 S2(f32(3), f32(7), f32(9));

     int ntotal = 0;
    
    GTimer T;T.Start();

    for( int i = 0; i < 20000000; i ++ )
    {
        Point.x += GMath::One();
        //GVector3 b = GDistance::ClosestPointTriangle(Point, S0 , S1, S2);

       GVector3 t(GMath::One());
       if( GCollision_Sphere::Sphere_Box( Point, GMath::One(), S0, S1,t, nullptr) )
       {
            ntotal += t.x.rawint32;
       }

       // ntotal += b.Max().rawint32;
    }

    T.End();



    std::cout << "time : " << T.GetDeltaTimeMS() << " ms  " <<ntotal;
}


