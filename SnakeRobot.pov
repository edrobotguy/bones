///////////////////////////////////////////////////////////////////////////////////
//    bones.inc  test program - snakebot
//  
//    by Ed Minchau
//    magicalrobotics.com
//    

/*
bones.inc version 1.01 for POV-Ray 3.5 or higher
Copyright (C) 2010 Ed Minchau

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
                                
                                
                                
Suggested povray.ini clock settings:

Initial_Frame = 1
Final_Frame = 30
Initial_Clock = 0.0
Final_Clock = 1.0
                         
                         
                         
testing a serpentine body 
completed animation can be seen at:
http://www.youtube.com/watch?v=_7OWtzCIERI
                         
                         
                         
*/
//////////////////// 
                      
#include "bones.inc"                          
                                           

camera { location <2.5,2,6> look_at <-5,0,-5>}      
light_source { <-1,12,5> color <1,1,1>}   
background { color <1,1,1> }  



  
//    textures

#declare coldSteel = texture{ pigment{color rgb<0.39, 0.41, 0.43>} 
             finish {  ambient 0.35   brilliance 2   diffuse 0.3  metallic   
             specular 0.80   roughness 1/20   reflection 0.1}};
#declare yellowBrass = texture{pigment{color rgb<0.65, 0.50, 0.25>} 
             finish {  ambient 0.35   brilliance 2   diffuse 0.3  metallic
             specular 0.80   roughness 1/20   reflection 0.1}}; 
 

    
 
//   body part objects


#macro RobotEye()
  merge{  
    sphere{<0,0,0>,1 texture{pigment{color rgb<1,1,1>}}}
    sphere{<0,0,1>,0.25 texture{pigment{color rgb<0,0,0>}}}
  }
#end  
    

#macro bodySeg()
  merge{
    sphere{<0,0,0>,0.25 texture {yellowBrass}}
    sphere{<0,0,1>,0.25 texture {yellowBrass}}
    cylinder { <0,0,0>,<0,0,1>,0.215 texture{coldSteel}}} 
#end
    
            
#macro HeadSkin()
  merge{
    cylinder{<0,0,0>,<0,0,1>,0.5 }
    intersection{
      cone{<0,0,1>,0.5,<0,0,2>,0.2}
      box{<-1,0,1>,<1,1,2>} } texture{coldSteel}}
#end
       

#macro JawSkin()
  intersection{
    cone{<0,0,0>,0.5,<0,0,1>,0.2}
    box{<-1,-1,0>,<1,0,1>}
    texture {yellowBrass}}
#end       

    
                        
                        
////////////////////////
//  sample snake robot skeleton 
////////////////////////
                                 

createSkeleton("snake")

// first the head

addFullBone("snake_Zero","Head",<0,180,0>,0.5,"HeadSkin()",<-30,-30,0>,<30,30,0>)
addFullBone("snake_Head","Jaw",<0,0,0>,0.5,"JawSkin()",<0,0,0>,<30,0,0>)
addBone("snake_Head","EyeStemR",<0,-90,0>,0.25,"InvisibleBone") 
addBone("snake_Head","EyeStemL",<0,90,0>,0.25,"InvisibleBone")
addFullBone("snake_EyeStemR","REye",<0,90,0>,0.15,"RobotEye()",
  <-60,30,0>,<60,150,0>)      
addFullBone("snake_EyeStemL","LEye",<0,-90,0>,0.15,"RobotEye()",
  <-60,-150,0>,<60,-30,0>) 
                              


// next the body segments

addFullBone("snake_Zero","Seg1",<0,0,0>,0.75,"bodySeg()",<-30,-30,-2>,<30,30,2>)

#declare Index = 2;
#while (Index<18)
  addFullBone(concat("snake_Seg",str(Index-1,1,0)),concat("Seg",str(Index,1,0)),
    <0,0,0>,0.75,"bodySeg()",<-15,-30,-2>,<15,30,2>)
  #declare Index = Index + 1;
#end

 
lockFreeBones("snake")

       
//  body poses  


#macro SlitherPose(SkelName,Phase)
  #local Index = 1;
  #while (Index<18) 
    #local Segment = concat(SkelName,"_Seg",str(Index,1,0));
    flexBoneYaw( Segment , (cosd(1080/17*Index-Phase)+1)/2 )
    #declare Index = Index + 1;
  #end                                                    
#end
                                                    



#macro Slither(SkelName,Time)
  interpolateMovement(SkelName,
    array[5]{"SlitherPose(SkelName,0)","SlitherPose(SkelName,90)",
      "SlitherPose(SkelName,180)","SlitherPose(SkelName,270)",
      "SlitherPose(SkelName,0)"},
    array[5]{0,0.25,0.5,0.75,1},Time)
#end

//  animation


Slither("snake",clock)
//  correction to the rotation of the skeleton while slithering
#declare RotVal = (getBoneRelRot("snake_Seg5").y*2+getBoneRelRot("snake_Seg4").y)/3;                                                  
rotateSkeleton("snake",<0,-120+RotVal,0>)
                                       


drawSkeleton("snake")
