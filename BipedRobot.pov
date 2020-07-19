///////////////////////////////////////////////////////////////////////////////////
//    bones.inc  test program - biped robot
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


                   
                   
testing a cartoony bipedal robot running 
animation can be seen at:
http://www.youtube.com/watch?v=U617Us7gfNM
              
              

 
*/
//////////////////// 
                      
#include "bones.inc"                          
                                           
//camera { location <0,4,12> look_at <0,2,0>}
camera { location <0,12,26> look_at <0,0,0>} 
//camera { location <0,7,4> look_at <0,1.8,1>}
//camera { location <0,5,6> look_at <0,1.8,1>} 
//camera { location <0,5,0> look_at <0,1.8,1>}  
//camera { location <-2,1.5,1.5> look_at <0,0.5,0.5>}

light_source { <-1,12,5> color <1,1,1>}   
//light_source { <-1,6,5> color <1,1,1>}   
background { color <1,1,1> }  
  
//    textures

#declare coldSteel = texture{ pigment{color rgb<0.39, 0.41, 0.43>} 
             finish {  ambient 0.35   brilliance 2   diffuse 0.3  metallic   
             specular 0.80   roughness 1/20   reflection 0.1}};
#declare yellowBrass = texture{pigment{color rgb<0.65, 0.50, 0.25>} 
             finish {  ambient 0.35   brilliance 2   diffuse 0.3  metallic
             specular 0.80   roughness 1/20   reflection 0.1}}; 
 
//   robot body parts
 
#declare FingerBone = merge{
    sphere { <0,0,0>, 0.25 texture{yellowBrass}}
    sphere { <0,0,1>, 0.25 texture{yellowBrass}}
    cylinder { <0,0,0>,<0,0,1>,0.215 texture{coldSteel}}} 
    
 

// some of the bones.inc macros have predefined abbreviations:
//
// GBL(boneLabel)             ==   getBoneLength(boneLabel) 
// GBLU(boneLabel)            ==   getBoneLengthUnscaled(boneLabel) 
// GBSC(boneLabel)            ==   getBoneScale(boneLabel)
// GBR(boneLabel)             ==   getBoneRot(boneLabel) 
// GBRR(boneLabel)            ==   getBoneRelRot(boneLabel)
// GBS(boneLabel)             ==   getBoneStart(boneLabel)
// GBE(boneLabel)             ==   getBoneEnd(boneLabel)
// GBM(boneLabel)             ==   getBoneMidpoint(boneLabel)
// GPOB(boneLabel,Fraction)   ==   getPointOnBone(boneLabel,Fraction)
// GBX(boneLabel)             ==   getBoneAxisX(boneLabel)
// GBY(boneLabel)             ==   getBoneAxisY(boneLabel)
// GBZ(boneLabel)             ==   getBoneAxisZ(boneLabel)
// IBL(partialBoneLabel)      ==   IndirectBoneLabel(partialBoneLabel)
// GBD(boneLabel1,boneLabel2) ==   getBoneDistance(boneLabel1,boneLabel2)
// GBEX(boneLabel)            ==   getBoneExtension(boneLabel)
// GBFX(boneLabel)            ==   getBoneFlex(boneLabel) 

// here are some more abbreviations to make the HandSkin() macro more compact:

#declare THP = "Thumb_Palm";
#declare IXP = "Index_Palm";
#declare MDP = "Middle_Palm";
#declare RNP = "Ring_Palm";
#declare PKP = "Pinky_Palm";

// end of abbreviations                             
                        
#macro HandSkin(Tex1,Tex2)
  #local Scale = GBSC(IBL(IXP));
  #local Scale3 = pow(Scale,3);
  #declare Scale = sqrt(Scale);

    blob{ 
      threshold (0.50 / Scale3)         
      cylinder{GBE(IBL(THP)),GBM(IBL(IXP)),0.2*Scale,1}        
      cylinder{GBE(IBL(IXP)),GPOB(IBL(THP),0.3),0.275*Scale,1}
      cylinder{GPOB(IBL(IXP),0.25),GPOB(IBL(THP),0.75),0.35*Scale,1}        
      cylinder{GBS(IBL(THP)),GPOB(IBL(IXP),0.65),0.29*Scale,1} 
      cylinder{GBS(IBL(THP)),GBE(IBL(THP)),0.25*Scale,1} 
      cylinder{GBS(IBL(IXP)),GPOB(IBL(THP),0.65),0.4*Scale,1}
      cylinder{GBS(IBL(IXP)),GBE(IBL(IXP)),0.275*Scale,1}
      cylinder{GBS(IBL(MDP)),GBE(IBL(MDP)),0.275*Scale,1}
      cylinder{GBS(IBL(RNP)),GBE(IBL(RNP)),0.275*Scale,1}
      cylinder{GPOB(IBL("ThumbOffset"),-0.25),GBE(IBL(PKP)),0.29*Scale,1}
      cylinder{GPOB(IBL(RNP),0.9),GPOB(IBL(MDP),0.9),0.23*Scale,1} 
      cylinder{GBM(IBL(PKP)),GPOB(IBL(IXP),0.75),0.2*Scale,1}
      sphere{GBS(IBL(THP)), GBL(IBL(IXP))/2.4, 1 texture{Tex1}}
      sphere{GPOB(IBL("ThumbOffset"),-0.1), GBL(IBL(IXP))/3, 1 texture{Tex1}} 
      texture{Tex2}
    }
#end 


#macro FingerSkin(Tex1,Tex2)
  blob{
    threshold 0.50
    cylinder{GBS(IBL("3")),GBE(IBL("3")),0.2,1 texture{Tex1}}    
    cylinder{GPOB(IBL("3"),0.25),GPOB(IBL("3"),0.8), 0.25, 1 texture{Tex2}}
    sphere{GBE(IBL("2")),0.25,1 texture{Tex1}}
    cylinder{GPOB(IBL("2"),0.3),GPOB(IBL("2"),0.8), 0.3, 1 texture{Tex2}}
    cylinder{GPOB(IBL("1"),0.2),GBE(IBL("1")),0.3,1 texture{Tex2}}
    sphere{GBS(IBL("1")), 0.315, 1 texture{Tex1}}
    sphere{GBE(IBL("1")), 0.275, 1 texture{Tex1}}
  }
#end 


#macro ThumbSkin(Tex1,Tex2)
  blob{
    threshold 0.50
    cylinder{GBS(IBL("2")),GBE(IBL("2")),0.2,1 texture{Tex1}}
    cylinder{GPOB(IBL("2"),0.35),GPOB(IBL("2"),0.7), 0.25, 1 texture{Tex2}}
    cylinder{GPOB(IBL("1"),0.2),GPOB(IBL("1"),0.8),0.3,1 texture{Tex2}}
    sphere{GBS(IBL("1")), 0.315, 1 texture{Tex1}}
    sphere{GBE(IBL("1")), 0.225, 1 texture{Tex1}}
  }
#end  


#macro RobotEye()
  merge{  
    sphere{<0,0,0>,1 texture{pigment{color rgb<1,1,1>}}}
    sphere{<0,0,1>,0.25 texture{pigment{color rgb<0,0,0>}}}
  }
#end  
    
        

#macro ArmBone(Tex1,Tex2)
   merge{
     cone{<0,0,0>, 0.2, <0,0,1>, 0.125 texture{Tex2}}
     sphere{<0,0,0>, 0.2 texture{Tex1}}
     sphere{<0,0,1>, 0.125 texture{Tex1}}
   }
#end 



#macro HeelSkin(Tex)
  sphere{<0,0,0>, 0.75 texture{Tex}}
#end                  

  
  
#macro HeadCyl(Height, Minor, Major)
  merge{
    cylinder{<0,0,0>,<0,0,Height>,Major}
    cylinder{<0,0,Minor>,<0,0,Height-Minor>,(Major+Minor)}
    torus{Major,Minor rotate 90*x translate (Height-Minor)*z}
    torus{Major,Minor rotate 90*x translate Minor*z}
  }
#end 



#macro ToeSkin(Tex)
  box{<-0.75,-0.5,0>,<0.75,0.01,1> texture{Tex}}
#end
    
                        
                        
////////////////////////
//  sample robot skeleton 
////////////////////////
                                 

//  fingers

createSkeleton("finger")

addBone("finger_Zero","Palm",<0,15,0>,1.4,"InvisibleBone") 
addFullBone("finger_Palm","1",<-30,0,0>,0.8,"InvisibleBone",<-90,0,0>,<0,0,0>)    
addFullBone("finger_1","2",<-30,0,0>,0.72,"InvisibleBone",<-90,0,0>,<0,0,0>)

copySkeleton("finger","Thumb") 
addFullBone("finger_2","3",<-45,0,0>,0.72,"InvisibleBone",<-90,0,0>,<0,0,0>)

scaleBoneXYZ("Thumb_1",<1.4,1.4,1>)
scaleBoneXYZ("Thumb_2",<1.4,1.4,1>)
attachObject("Thumb_Zero","ThumbSkin(yellowBrass,coldSteel)") 
attachObject("finger_Zero","FingerSkin(yellowBrass,coldSteel)")

copySkeleton("finger","Middle")
copySkeleton("finger","Ring")
copySkeleton("finger","Pinky") 

renameSkeleton("finger","Index") 
                                            
setBoneRotation("Thumb_Palm",<-22,15,50>)
setBoneLength("Thumb_Palm",1)
setBoneRotation("Thumb_2",<-19,0,0>)
setBoneRotation("Middle_Palm",<0,0,0>)
setBoneRotation("Ring_Palm",<0,-15,0>)
setBoneRotation("Pinky_Palm",<0,-30,0>)  

limitBoneRot("Thumb_Palm",<-60,-15,50>,<0,22,50>)
lockBoneRot("Index_Palm")  
lockBoneRot("Middle_Palm")
lockBoneRot("Ring_Palm")
lockBoneRot("Pinky_Palm")

scaleSkeleton("Thumb",0.79)
scaleSkeleton("Pinky",0.85)
scaleSkeleton("Ring",0.95)
scaleSkeleton("Middle",1.05) 

//  hand
                    
createSkeleton("Hand") 
addFullBone("Hand_Zero","Wrist",<0,0,0>,0.5,"FingerBone",<-90,-15,-90>,<90,15,90>)  
addBone("Hand_Wrist","ThumbOffset",<-15,80,0>,0.5,"InvisibleBone")
lockBoneRot("Hand_ThumbOffset")

joinSkeletons("Thumb","Hand_ThumbOffset") 
joinSkeletons("Index","Hand_Wrist")
joinSkeletons("Middle","Hand_Wrist") 
joinSkeletons("Ring","Hand_Wrist")
joinSkeletons("Pinky","Hand_Wrist") 
              
attachObject("Hand_Zero","HandSkin(yellowBrass,coldSteel)")  

//  arms  
                  
createSkeleton("Arm")
addFullBone("Arm_Zero","Upperarm",<0,-75,0>,4,"ArmBone(yellowBrass,coldSteel)",
  <-165,-90,-75>,<75,60,75>)
addFullBone("Arm_Upperarm","Forearm",<-45,0,0>,3.5,"ArmBone(yellowBrass,coldSteel)",
  <-105,0,0>,<-15,0,0>)
joinSkeletons("Hand","Arm_Forearm") 
rotateBone("Arm_Hand_Zero",<0,0,90>)

copySkeleton("Arm","LArm")
renameSkeleton("Arm","RArm")
mirrorSkeleton("LArm")  

//  torso

createSkeleton("Body")
addBone("Body_Zero","Backbone",<-90,0,180>,5,"FingerBone")
addBone("Body_Backbone","RCollar",<0,-90,0>,3,"FingerBone")
addBone("Body_Backbone","LCollar",<0,90,0>,3,"FingerBone")

joinSkeletons("RArm","Body_RCollar")
joinSkeletons("LArm","Body_LCollar")    

addBone("Body_Zero","RHip",<0,90,0>,2,"FingerBone")
addBone("Body_Zero","LHip",<0,-90,0>,2,"FingerBone")
                                        
// legs                                        
                                        
createSkeleton("Leg")
addFullBone("Leg_Zero","Thigh",<60,-90,0>,6,"ArmBone(yellowBrass,coldSteel)",
  <-60,-105,-45>,<135,-60,15>)
addFullBone("Leg_Thigh","Calf",<45,0,0>,5,"ArmBone(yellowBrass,coldSteel)",
  <0,0,0>,<150,0,0>)
addFullBone("Leg_Calf","Heel",<-105,0,0>,1,"HeelSkin(yellowBrass)",
  <-105,0,-15>,<-75,0,15>)
addFullBone("Leg_Heel","Foot",<15,0,0>,2,"FingerBone",<-105,0,0>,<60,0,0>)
addFullBone("Leg_Foot","Toes",<0,0,0>,0.85,"ToeSkin(yellowBrass)",
  <-60,0,0>,<15,0,0>)

copySkeleton("Leg","LLeg")
renameSkeleton("Leg","RLeg")
mirrorSkeleton("LLeg")

joinSkeletons("RLeg","Body_RHip")  
joinSkeletons("LLeg","Body_LHip")   

//  neck and head

addFullBone("Body_Backbone","Neck",<0,0,0>,1.5,"ArmBone(yellowBrass,coldSteel)",
  <-90,-15,-90>,<60,15,90>) 
scaleBoneXYZ("Body_Neck",<3,3,1>)

addBone("Body_Neck","Head",<0,0,0>,2,"HeadCyl(1.618,0.2,0.3) texture{coldSteel}")
scaleBoneXYZ("Body_Head",<3,2,1.125>)

addBone("Body_Head","EyeStem",<-90,0,0>,1.5,"InvisibleBone")
addBone("Body_EyeStem","EyeStemR",<0,-90,0>,0.65,"InvisibleBone")                                 
addBone("Body_EyeStem","EyeStemL",<0,90,0>,0.65,"InvisibleBone") 
addFullBone("Body_EyeStemR","REye",<0,90,0>,0.75,"RobotEye()",
  <-60,30,0>,<60,150,0>)      
addFullBone("Body_EyeStemL","LEye",<0,-90,0>,0.75,"RobotEye()",
  <-60,-150,0>,<60,-30,0>)
  
lockFreeBones("Body")
                     
                     
                     
/////////////////////
       
//  body poses

#macro PackBody()
  extendBone(IBL("RLeg_Thigh"),<0,0.82,0.25>)
  flexBone(IBL("RLeg_Calf"),<0,0,0>)
  flexBonePitch(IBL("RLeg_Heel"),0)
  extendBone(IBL("RLeg_Foot"),<0,0,0>)
  extendBone(IBL("RArm_Forearm"),<1/3,0.5,0.5>)
  extendBone(IBL("RArm_Hand_Wrist"),<1/6,0.5,1/3>)   
  extendBone(IBL("LLeg_Thigh"),<0,0.82,0.25>)
  flexBone(IBL("LLeg_Calf"),<0,0,0>) 
  flexBonePitch(IBL("LLeg_Heel"),0)
  extendBone(IBL("LLeg_Foot"),<0,0,0>)
  extendBone(IBL("LArm_Forearm"),<1/3,0.5,0.5>)
  extendBone(IBL("LArm_Hand_Wrist"),<1/6,0.5,1/3>) 
  extendBone(IBL("Neck"),<0,0.5,1>)
#end     


#macro StandBody()
  extendBone(IBL("RLeg_Thigh"),<0.7,0.33,0.75>)
  flexBonePitch(IBL("RLeg_Calf"),0.9)
  flexBonePitch(IBL("RLeg_Heel"),1)
  extendBonePitch(IBL("RLeg_Foot"),0.64)   
  extendBone(IBL("RArm_Upperarm"),<0.6875,0.1,0.5>)
  extendBone(IBL("RArm_Forearm"),<2/3,0.5,0.5>)
  extendBone(IBL("RArm_Hand_Wrist"),<0.5,0.5,0.5>)   
  extendBone(IBL("LLeg_Thigh"),<0.7,0.33,0.75>)
  flexBonePitch(IBL("LLeg_Calf"),0.9)
  flexBonePitch(IBL("LLeg_Heel"),1)
  extendBone(IBL("LLeg_Foot"),<0.64,0,0>)   
  extendBone(IBL("LArm_Upperarm"),<0.6875,0.1,0.5>)
  extendBone(IBL("LArm_Forearm"),<2/3,0.5,0.5>)
  extendBone(IBL("LArm_Hand_Wrist"),<0.5,0.5,0.5>) 
  extendBone(IBL("Neck"),<0.6,0.5,0.5>)
#end



#macro BendBackWrists()
  extendBone(IBL("RArm_Hand_Wrist"),<2/3,0.5,0.5>)   
  extendBone(IBL("LArm_Hand_Wrist"),<2/3,0.5,0.5>)  
#end 



#macro PropUp() 
  extendBone(IBL("RArm_Upperarm"),<0.9375,0.1,0.5>)
  extendBone(IBL("RArm_Forearm"),<5/6,0.5,0.5>)  
  extendBone(IBL("LArm_Upperarm"),<0.9375,0.1,0.5>)
  extendBone(IBL("LArm_Forearm"),<5/6,0.5,0.5>)
  extendBone(IBL("RArm_Hand_Wrist"),<11/12,0.5,0.5>)   
  extendBone(IBL("LArm_Hand_Wrist"),<11/12,0.5,0.5>)  
#end  

//  partial poses for running movement

#macro RunLeg1(LorR)
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Leg_Thigh")),<0.75,0.33,0.75>)
  flexBonePitch(IBL(concat(LorR,"Leg_Calf")),0.6)
  flexBonePitch(IBL(concat(LorR,"Leg_Heel")),1)
  extendBonePitch(IBL(concat(LorR,"Leg_Foot")),0.5)
  extendBonePitch(IBL(concat(LorR,"Leg_Toes")),0.8)
#end
       
       
#macro RunLeg2(LorR)
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Leg_Thigh")),<0.35,0.33,0.75>)
  flexBonePitch(IBL(concat(LorR,"Leg_Calf")),0.4)
  flexBonePitch(IBL(concat(LorR,"Leg_Heel")),1)
  extendBonePitch(IBL(concat(LorR,"Leg_Foot")),0.8)
  extendBonePitch(IBL(concat(LorR,"Leg_Toes")),0.8)
#end


#macro RunLeg3(LorR)
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Leg_Thigh")),<0.85,0.33,0.75>)
  flexBonePitch(IBL(concat(LorR,"Leg_Calf")),0.3)
  flexBonePitch(IBL(concat(LorR,"Leg_Heel")),1)
  extendBonePitch(IBL(concat(LorR,"Leg_Foot")),0.9)
  extendBonePitch(IBL(concat(LorR,"Leg_Toes")),0)
#end


#macro RunLeg4(LorR)
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Leg_Thigh")),<0.45,0.33,0.75>)
  flexBonePitch(IBL(concat(LorR,"Leg_Calf")),0.8)
  flexBonePitch(IBL(concat(LorR,"Leg_Heel")),1)
  extendBonePitch(IBL(concat(LorR,"Leg_Foot")),0.73)
  extendBonePitch(IBL(concat(LorR,"Leg_Toes")),0.8)
#end 


#macro RunArm1(LorR)      
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Arm_Upperarm")),<0.75,0.1,0.5>)
  extendBonePitch(IBL(concat(LorR,"Arm_Forearm")),1/3)
#end     


#macro RunArm2(LorR)      
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Arm_Upperarm")),<0.625,0.1,0.5>)
  extendBonePitch(IBL(concat(LorR,"Arm_Forearm")),1/3)
#end


#macro RunArm3(LorR)      
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Arm_Upperarm")),<0.8,0.1,0.5>)
  extendBonePitch(IBL(concat(LorR,"Arm_Forearm")),0.5)
#end


#macro RunArm4(LorR)      
// LorR is a string containing either "L" or "R"
  extendBone(IBL(concat(LorR,"Arm_Upperarm")),<0.5,0.1,0.5>)
  extendBonePitch(IBL(concat(LorR,"Arm_Forearm")),1/6)
#end


  
  
  
/////////////////////  

// combination poses 

#macro Unpacking_A()
  PackBody()
  BendBackWrists()
#end


#macro Unpacking_B()
  PackBody()
  PropUp()
#end           


#macro Run1()
  RunLeg1("R")
  RunArm2("R")
  RunLeg2("L")
  RunArm1("L")
#end


#macro Run2()
  RunLeg3("R")
  RunArm4("R")
  RunLeg4("L")
  RunArm3("L")
#end           


#macro Run3()
  RunLeg1("L")
  RunArm2("L")
  RunLeg2("R")
  RunArm1("R")
#end


#macro Run4()
  RunLeg3("L")
  RunArm4("L")
  RunLeg4("R")
  RunArm3("R")
#end




// movements

#macro UnpackRobot(SkelName,Time)
  interpolateMovement(SkelName,
    array[4]{"PackBody()","Unpacking_A()","Unpacking_B()","StandBody()"},
    array[4]{0,0.2,0.4,1},Time)
#end         


#macro RunBiped(SkelName,Time)
  interpolateMovement(SkelName,
    array[5]{"Run1()","Run2()","Run3()","Run4()","Run1()"},
    array[5]{0,0.25,0.5,0.75,1},Time)
#end


#macro StandToRun(SkelName,Time)
  interpolatePose(SkelName,"StandBody()","Run1()",Time)
#end


#macro StartRunning(SkelName)
  #if (clock<0.2)
    StandToRun("Body",clock*5)
  #else
    RunBiped("Body",(clock - 0.2)*1.25)
  #end
#end 


#macro Walk1(Speed)   
// same as StandToRun, expressed as a pose rather than a movement
// this interpolated pose is part of an interpolated movement, so we don't 
// directly state which skeleton is involved but just pass an empty string.
// the skeleton name will be stated in the WalkBiped interpolation
  interpolatePose("","StandBody()","Run1()",Speed)
#end  


#macro Walk2(Speed)
  interpolatePose("","StandBody()","Run2()",Speed)
#end


#macro Walk3(Speed)
  interpolatePose("","StandBody()","Run3()",Speed)
#end


#macro Walk4(Speed)
  interpolatePose("","StandBody()","Run4()",Speed)
#end 


#macro WalkBiped(SkelName,Speed,Time)
  interpolateMovement(SkelName,
    array[5]{
      "Walk1(Speed)","Walk2(Speed)","Walk3(Speed)","Walk4(Speed)","Walk1(Speed)"},
    array[5]{0,0.25,0.5,0.75,1},Time)
#end

/////////////////////

// animation       
       

//UnpackRobot("Body",clock)                                 
//rotateSkeleton("Body",<0,-60*clock,0>)
 
  
//StartRunning("Body")
//rotateSkeleton("Body",<0,-60,0>)


WalkBiped("Body",0.3,clock)
rotateSkeleton("Body",<0,-60,0>)

 
drawSkeleton("Body")
