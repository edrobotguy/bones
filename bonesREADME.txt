/////////////////////////////////////////////////////////////////////////////////// 

bones.inc version 1.01 for POV-Ray 3.5 or higher
Copyright (C) 2010 Ed Minchau http://www.magicalrobotics.com

Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.

You should have received a copy of the GNU Free Documentation License
along with this document. If not, see <http://www.gnu.org/licenses/>.


///////////////////////////////////////////////////////////////////////////////////




                                 Using bones.inc
 


                                        
///////////////////////////////////////////////////////////////////////////////////
                                        

//////////////  Contents

1.1 introduction
1.2 Names and Labels
1.3 the Zero bone and bone hierarchy
1.4 bone coordinate system                                        

2.1 creating a new skeleton
2.2 adding a bone to a skeleton
2.3 copying, joining, and mirroring skeletons
2.4 renaming a bone or skeleton
2.5 deleting a bone or skeleton
2.6 attaching POVray objects to bones 
 
3.1 bone information macros
3.2 the IndirectBoneLabel macro
3.3 abbreviations
3.4 skeleton information macros

4.1 rotating bones
4.2 bone rotation limits
4.3 scaling a bone or skeleton
4.4 rotating a skeleton
4.5 moving a skeleton
4.6 animating a skeleton: interpolating poses and movements 

5.1 visualizing bones and skeletons
5.2 drawing POV objects attached to bones and skeletons

6.1 reserved keywords 
6.2 the bone and skeleton arrays
6.3 diagnostics
6.4 background macros
6.5 future expansion and bug reports

7.1 index of bones.inc keywords and commands
                                              
                                              
                                              
                                              
///////////////////////////////////////////////////////////////////////////////////

 
 

//////////////  

//////////////  1.1 introduction   


Bones and Skeletons are conceptual objects onto which POVray objects are attached.
Bones are connected together to form skeletons, and the position and orientation 
of the skeleton and the length of and angles between the bones determine the final
position and orientation of the attached POV object.  Bones and skeletons may be
copied and joined together to make more complex skeletons, and the attached POV 
objects and other properties are inherited.  A skeleton can have many Poses and so
can be animated by interpolating between Poses.  




//////////////  1.2 Names and Labels  


Skeletons and bones are indexed in two ways: 
- by index number, internally within bones.inc
- by Label, externally (ie by the end user)

Skeleton names consist of only the characters 0..9,a..z,A..Z and are case-sensitive.
When a new bone is created, its name can likewise only consist of the characters
0..9,a..z,A..Z and is also case-sensitive.  Every bone in a skeleton must have a 
unique name; when a new bone or skeleton is created or renamed, bones.inc will 
verify that the name is valid and not already in use.

A bone Label consists of the skeleton name, followed by an underscore, and then the 
name of the bone within the skeleton.  Each bone within a skeleton has a unique 
name, and each skeleton has a unique name, so every bone in the scene has a unique
Label. 

It is a good idea to keep your bone and skeleton names as short as possible, while 
still retaining sufficient meaning to the names.  As you build up skeletons by 
joining them together, skeleton names will be prepended to bone names and thus bone
labels may become quite lengthy.  Once a complex skeleton is completed, you may find
it convenient to declare some short variable names to contain some common lengthy
bone label strings, or to rename bones with short names.




//////////////  1.3 the Zero bone and bone hierarchy 


When a skeleton is created, the first bone of the skeleton is automatically created.
This bone named "Zero" has a length of zero, with only a position and orientation in
POVray's coordinate system.  Automatically attached to this bone is the POV object 
InvisibleBone, a completely transparent sphere with radius 1.  Translation and 
rotation of the "Zero" bone moves and rotates the entire skeleton.

Bones are linked together in a hierarchy; each bone has a Parent bone.  The Zero 
bone of a skeleton is its own Parent.  The first bone a user adds to a newly-created
skeleton will have as its Parent the Zero bone of that skeleton.  

A bone may have any number of child bones, but each has only one Parent bone.  The
ending point of the Parent bone is considered the starting point of each of its 
child bones.  The orientation of the Parent bone is the initial orientation of the
child bone before its own rotations.  As a parent bone is translated and rotated, 
the child bone follows its movements.

There is also a "Zero" skeleton, containing only one bone labelled "Zero_Zero".  
The Zero skeleton is only used to initialize the skeleton and bone arrays, and no
further bone or skeleton operations are allowed on the Zero skeleton.

                                
                                

//////////////  1.4 bone coordinate system 


Each bone has its own 3d orthogonal coordinate system, separate from POVray's
coordinate system.  This coordinate system is represented by three unit vectors in
POVray's coordinate system; the AxisX, AxisY, and AxisZ vectors describe the bone's
coordinate system, with the AxisZ along the length of the bone and the AxisX and 
AxisY giving a unique "Roll" value around AxisZ.  (Only two of the three axes are
really required to translate from a bone's coordinate system to POVray's system, 
but all three are included because it just makes life easier for programmers.)

The coordinate system of a bone, and hence its orientation within POVray's
coordinate system, is dependent on two factors:
- the orientation (given by AxisX, AxisY, and AxisZ) of the Parent bone
- the Pitch, Yaw, and Roll of a bone relative to its Parent

When calculating the coordinate system of a bone, bones.inc first considers the
coordinate system of the Parent bone to be the default.  Then these axes are changed
by first changing the Pitch and then the Yaw of the child bone relative to its
Parent.  Then the bone is rotated about its own new AxisZ by an amount equal to
the Roll.  These rotations follow the same left-handed rotation as POVray, and all
rotation angles are expressed in degrees.
                                           
Each bone has a Length and a Scale.  When a bone is newly created, its Scale
value is automatically set to 1; after that the bone may be scaled as desired.  A
skeleton may also be scaled - bones.inc does this by scaling the Zero bone of the 
skeleton.  To arrive at the final rendered length of the bone, bones.inc multiplies
the bone's Length by its Scale by its skeleton's Scale. 

Any time an operation (such as a translate, rotate, scale, and others) is performed
on a bone or skeleton, bones.inc's background macros recalculate the position and
orientation of all affected bones.  Each bone has Starting, Ending, and AbsRot
parameters which keep track of the starting point, ending point, and rotation as
expressed in POVray's coordinate system.
                                           
Objects attached to bones are defined as if there was a bone starting at <0,0,0> and 
ending at <0,0,1>.  The coordinate system of that virtual bone would be identical to 
POVray's coordinate system, i.e.:

  AxisX = x = <1,0,0>
  AxisY = y = <0,1,0>
  AxisZ = z = <0,0,1>              

When the bone or skeleton is finally drawn, this attached BoneObject is parsed and
then scaled, rotated, and translated to match the bone.
                                                                                   
                                                                                   
                                                                                   
                                                                                   
///////////////////////////////////////////////////////////////////////////////////

 
 

//////////////      

//////////////  2.1 creating a new skeleton 


When we create a new skeleton, we assume that its Zero bone is the only bone in the 
new skeleton.  This Zero bone's orientation matches POVray's coordinate system, and
its other parameters are filled in by default.  All bones.inc requires is the name
of the new skeleton:

        createSkeleton (SkelName)
// creates a new skeleton and its Zero bone

The new skeleton's name SkelName is first checked to see that it is a valid name, 
and that it is not already used for another skeleton.  If that's ok, it creates
space in the bone and skeleton arrays for the new skeleton and new Zero bone and
fills in the appropriate data.


//////////////  2.2 adding a bone to a skeleton 


New bones may be added to a skeleton anywhere there is an existing bone.  That bone
is the Parent bone of the new bone, and the new bone will have as its starting point
the ending of the Parent bone.  It will also initially have the same coordinate
system as the Parent bone until its own Pitch,Yaw, and Roll are applied.

Each bone must have a unique name within a skeleton.  Two bones in different 
skeletons may have the same name, but since skeleton names are unique the bones will
have different labels.

There are three ways to add bones:

        QuickBone( ParentLabel, BoneName)
// ParentLabel is the label of the bone that will be the parent of the new bone
// BoneName must be unique within that skeleton
// just declaring a new bone, other parameters set to default values


        addBone( ParentLabel, BoneName, PYR, BoneLen, BoneObj)
// ParentLabel is the label of the bone that will be the parent of the new bone
// BoneName must be unique within that skeleton
// PYR is the pitch, yaw, and roll of the bone relative to its parent
// BoneLen is the length (scale defaults to 1) independent of skeleton scale
// BoneObj contains a string that will be parsed when the bone is drawn   

 
        addFullBone( ParentLabel, BoneName, PYR, BoneLen, BoneObj, MinPYR, MaxPYR)
// same as addBone, but with limits on bone rotation defined as follows:
// MinPYR is a 3d vector with the lower limits of the pitch, yaw, and roll
// MaxPYR is a 3d vector with the upper limits of the pitch, yaw, and roll




//////////////  2.3 copying, joining, and mirroring skeletons 


The bones.inc macros which copy skeletons, join them together, or mirror-image a 
skeleton about the x=0 axis are as follows:

        copySkeleton(oldSkelName,newSkelName)

The copySkeleton macro creates a new skeleton with the name newSkelName, and 
populates it with a duplicate of the skeleton oldSkelName.  The only difference will
be in the Parent pointers and the skeleton pointers.

        joinSkeletons(ChildSkelName,ParentBoneLabel) 

The joinSkeletons macro first creates a copy of the child skeleton.  It then changes
the Parent of the Zero bone in the copy of the child skeleton from itself to the 
bone index number of the new parent bone (derived from the ParentBoneLabel).  All 
pointers from the bones to the skeleton in the copy of the child skeleton are
changed to the new parent skeleton.  The old child skeleton is then deleted, as are
the references in the skeleton arrays to the child skeleton and its copy.

When a skeleton is joined to a bone on another skeleton, all the bones in the child
skeleton are renamed to include the child skeleton name.  For instance, if a bone 
has the label "RHand_Index_3", and the "RHand" skeleton is joined to the skeleton 
"RArm" at the bone labelled "RArm_Forearm", the entire RHand skeleton is renamed, 
and our bone "RHand_Index_3" will acquire the new label "RArm_RHand_Index_3"; each 
bone of the old RHand skeleton thus becomes part of the RArm skeleton.  References
to a skeleton named RHand after that join operation would produce a "skeleton not
found" error. 

        mirrorSkeleton(SkelName)

The mirrorSkeleton macro multiplies all the Yaw and Roll values in the skeleton
SkelName by -1.  The minimum/maximum rotation values (see section 4.2 below) are
automatically adjusted to reflect (heh) the new orientation of the bone. 
 
This will cause the addition of a few degrees of Yaw or Roll to a bone to have the 
opposite effect on the bone's mirror image.  However, the flexBone and extendBone
operations (section 4.1) have identical effects on the bone's rotation whether a 
bone has been mirrored or not.  The same flex and extend commands can thus be used
on both a skeleton and its mirror image and give the same results visually, even 
though the Yaw and Roll parameter values will be very different.

The mirror-imaging is of the rotations of the skeleton bones only - not the POVray
objects attached to them (section 2.6).  If the attached object is not mirror-
symmetrical about the plane x=0 then the objects attached to the mirrored skeleton
will not look the way you want them to, and indeed could look bizarre.




//////////////  2.4 renaming a bone or skeleton
  

        renameBone(boneLabel,newName)
                               
The renameBone macro first finds a bone from the boneLabel, then checks to see if
the new name is valid and not already in use within that skeleton.  If so, it 
replaces the old bone name with the new name.  For example, if the bone's skeleton
name is "SkelName" and the bone's new name is "newName", the bone's new Label will
be "SkelName_newName".

                               
        renameSkeleton(oldName,newName)

The renameSkeleton macro first finds the skeleton with the name oldName, then
checks to see if the newName is valid and unused.  If so, it replaces the old name
with the new name.  All of the bones in that skeleton automatically acquire new
Labels.      




//////////////  2.5 deleting a bone or skeleton 


        deleteBone(boneLabel) 

The deleteBone macro deletes a bone and any child bones of that bone.  

        deleteSkeleton(SkelName)

The deleteSkeleton macro removes a skeleton and all its bones from the data arrays.

The deleteBone macro, deleteSkeleton macro, and joinkeletons macro all call the
appropriate destructor macros and perform automatic "garbage collection", keeping
the bone and skeleton arrays as orderly (i.e. all Parent bones have lower index
numbers than their children) and as short as possible.




//////////////  2.6 attaching POVray objects to bones


The POVray objects attached to bones must be expressed as strings, like this:

// option A
#declare sampleBone = 
  cylinder { <0,0,0>,<0,0,1>,0.15 
  texture{pigment{color rgb<0.65, 0.50, 0.25>} }
  
attachObject("Finger_2","sampleBone") 
//

or like this:

// option B
#declare sampleBone = concat(
  "cylinder { <0,0,0>,<0,0,1>,0.15 ",
  "texture{pigment{color rgb<0.65, 0.50, 0.25>} }";
  
attachObject("Finger_2",sampleBone) 
//

or like this:

// option C
#macro sampleBone()
  #local skinTex = texture{pigment{color rgb<0.65, 0.50, 0.25>}
  merge{
    sphere { <0,0,0>, 0.35 texture{skinTex}}
    sphere { <0,0,1>, 0.35 texture{skinTex}}
    cylinder { <0,0,0>,<0,0,1>,0.3 texture{skinTex}}
  }
#end

attachObject("Finger_2","sampleBone()")
//    

or like this:

// option D
#macro sampleBone()   // note skinTex must be defined elsewhere
  concat(                                                         
  "merge{",
   " sphere { <0,0,0>, 0.35 texture{skinTex}}",
   " sphere { <0,0,1>, 0.35 texture{skinTex}}",
   " cylinder { <0,0,0>,<0,0,1>,0.3 texture{skinTex}}}")
#end

attachObject("Finger_2",sampleBone())
//  


Of the options above, option A or B can be used on all regular bones, and option A
is the preferred method.  Option C or D must be used on any bone whose object 
contains calls to bones.inc macros like IndirectBoneLabel (section 3.2), and so this
is a must for any object attached to a Zero bone (other than "InvisibleBone"), but 
is not necessarily needed for other bones; they can either be declared as POVray
objects or as macros, as shown above.  

I must reiterate that option C - declaring the attached objects as macros and 
attaching the macro name as a string - will work on all bone objects.

The object or macro string is stored in a BoneObject array, and is only parsed when
the bone "Finger_2" is finally drawn.  This attached object or macro follows a bone
around as the bone is copied or as the "Finger" skeleton is joined to another 
skeleton. 

When the string is finally parsed by the drawBone macro, drawSkeleton macro, or
drawAllBones macro, the object in the string is scaled, rotated and translated to
match the bone to which it is attached.  There is one exception: any objects 
attached to a Zero bone are not scaled, rotated, or translated.  This is because the
Zero bone can take advantage of the IndirectBoneLabel macro (see section 3.2 below).

The string containing an object name or macro name may be attached to the bone at
the time the bone is created, or it may be attached later on using the attachObject
macro.  

The attachObject(boneLabel,ObjectNameString) macro has an inverse operation:     


        detachObject(boneLabel) 
// equivalent to attachObject(boneLabel,"InvisibleBone").
                                                         
                                                         
                                                         
                                                         
///////////////////////////////////////////////////////////////////////////////////




//////////////       
                                 
//////////////  3.1 bone information macros
                       

There are a number of parameters associated with each bone which are accessible to
the user.  Some of these are primary parameters stored in the bone arrays, and
others are derived scondary parameters.  All are accessible to the user through the 
following macros:
                                                                        
                                                                        
        getBoneLength (boneLabel)
// length of bone including bone scale and skeleton scale factors

      
        getBoneLengthUnscaled (boneLabel)
// raw bone length 
                                                       

        getBoneScale (boneLabel) 
// bone scaling factor multiplied by skeleton scaling factor 


        getBoneScaleXYZ (boneLabel)
// 3D bone attached object scaling factor

      
        getBoneRot(boneLabel) 
// rotation <X,Y,Z> using povray's rotation vectors

         
        getBoneRelRot(boneLabel)              
// rotation <Pitch,Yaw,Roll> relative to parent bone


        getBoneExtension (boneLabel)      
// returns a 3d vector with the fractional extension values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll


        getBoneFlex (boneLabel)  
// returns a 3d vector with the fractional flexion values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll


        getBoneMinRot (boneLabel) 
// returns 3d vector representing the negative limit of pitch, yaw and roll


        getBoneMaxRot (boneLabel)
// returns 3d vector representing the positive limit of pitch, yaw and roll

       
        getBoneStart(boneLabel)
// returns the POVray XYZ coordinates of the starting point of the bone

        
        getBoneEnd(boneLabel)  
// returns the POVray XYZ coordinates of the ending point of the bone

         
        getBoneMidpoint(boneLabel)  
// returns the POVray XYZ coordinates of the midpoint of the bone

    
        getPointOnBone(boneLabel,Fraction)  
// Fraction can be any floating point value
// if Fraction = 0, returns the Starting point of the bone 
// if Fraction = 1, returns the Ending point of the bone
// if Fraction = 0.5, returns the Midpoint of the bone 
// Fraction values greater than 1 return a point outside the bone beyond the Ending
// Fraction values less than 0 return a point outside the bone beyond the Starting


        getBoneAxisX(boneLabel) 
// returns a unit-length vector along the X axis of the bone's coordinate system

         
        getBoneAxisY(boneLabel) 
// returns a unit-length vector along the Y axis of the bone's coordinate system  

        
        getBoneAxisZ(boneLabel) 
// returns a unit-length vector along the Z axis of the bone's coordinate system 
       

        getBoneDistance(boneLabel1,boneLabel2) 
// returns the distance between the closest two points located between the Starting
// and Ending points on two bones (useful for collision detection)

                      
                       
                                                               
//////////////  3.2 the IndirectBoneLabel macro 


        IndirectBoneLabel(partialBoneLabel)
                         
                            
IndirectBoneLabel receives as its parameter a partial bone label, which is combined 
with the label of the Zero bone to produce a final bone label.  In this way, the 
string-contained object attached to the Zero bone can reference any bone below the
Zero bone in the hierarchy.  The macros getBoneStart, getBoneEnd, getBoneMidpoint,
and getPointOnBone can use this indirectly-generated bone label to return the
coordinates needed to produce the POV object attached to the Zero bone.

For instance, if one creates a skeleton of a hand, it is easy to attach simple POV
objects to individual bones; just declare a FingerBone string containing the finger
segment object, attach it to the bones, and you're done.  The webbing between the
thumb and the index finger, however, is attached to two bones (let's call them
"Hand_Index_Palm" and "Hand_Thumb_Palm").  To create a blob object that represents
the hand web, we need to find a Zero bone that is higher in the bone hierarchy than
both bones (in this case, "Hand_Zero").  Then we can use the bone information of any
bone below our Zero bone in the hierarchy to generate our attached blob object.

//
#macro HandWeb() 
  blob{ 
    threshold 0.60 
    cylinder{getBoneEnd(IndirectBoneLabel("Thumb_Palm")),
      getBoneMidpoint(IndirectBoneLabel("Index_Palm")),0.25,1}, 
    cylinder{getBoneEnd(IndirectBoneLabel("Index_Palm")),
      getBoneMidpoint(IndirectBoneLabel("Thumb_Palm")),0.25,1}, 
    cylinder{getBoneStart(IndirectBoneLabel("Thumb_Palm")),
      getPointOnBone(IndirectBoneLabel("Index_Palm"),0.75),0.25,1}, 
    texture{skinTex}    //  skinTex should be defined previously
    }    
#end

...

attachObject("Hand_Zero","HandWeb()")
//       
       
It is this IndirectBoneLabel macro which forces us to attach objects contained 
within strings to the bones, rather than the objects themselves.  If the above 
HandWeb object were to be attached to the Hand_Zero bone, POVray's parser would
evaluate IndirectBoneLabel as soon as HandWeb was declared.  The getBoneMidpoint and
other such macros would not be able to find the bones to which they refer.

However, by containing the object within a string, we defer evaluation of the
IndirectBoneLabel macro until after the skeletons are defined and the bones are
actually drawn. 
  
Now, when the HandWeb object is attached to "Hand_Zero", the string follows that
Zero bone around as the skeletons are copied, joined, moved around, and so on.  
The skeleton Hand might be copied and mirror-imaged to give LHand and RHand
skeletons, and those joined to a "Body" skeleton at the ends of the arms, so the
"Hand_Zero" bone's label will change throughout these join and copy operations.  Its
position and orientation will change as the skeleton is moved and manipulated.  When
we finally call the drawSkeleton macro, the HandWeb string will be parsed when the
right hand and left hand are drawn.

Suppose the "Hand_Zero" bone has been copied, renamed, and then joined to a "Body"
skeleton, a copy of which is named "Frodo".  As the drawSkeleton("Frodo") macro is
running, it will encounter a bone labelled "Frodo_LArm_LHand_Zero" - the skeleton 
name is "Frodo" and the bone name is "LArm_LHand_Zero".  That bone has attached to
it the "HandWeb()" string.  When the IndirectBoneLabel("Thumb_Palm") macro is 
finally called, it returns the bone label "Frodo_LArm_LHand_Thumb_Palm".  

In this way the entire HandWeb is parsed with the correct endpoints for the blob 
objects generated by IndirectBoneLabel and the other bone information macros.




//////////////  3.3 abbreviations

Many of the bone information macros have fairly long names, and would be used 
together for creating the macros attached to Zero bones.  Rather than having to type
out the full name each time, bones.inc includes the following abbreviations

GBL(boneLabel)             ==   getBoneLength (boneLabel) 
GBLU(boneLabel)            ==   getBoneLengthUnscaled (boneLabel) 
GBSC(boneLabel)            ==   getBoneScale (boneLabel)
GBR(boneLabel)             ==   getBoneRot(boneLabel) 
GBRR(boneLabel)            ==   getBoneRelRot(boneLabel)
GBS(boneLabel)             ==   getBoneStart(boneLabel)
GBE(boneLabel)             ==   getBoneEnd(boneLabel)
GBM(boneLabel)             ==   getBoneMidpoint(boneLabel)
GPOB(boneLabel,Fraction)   ==   getPointOnBone(boneLabel,Fraction)
GBX(boneLabel)             ==   getBoneAxisX(boneLabel)
GBY(boneLabel)             ==   getBoneAxisY(boneLabel)
GBZ(boneLabel)             ==   getBoneAxisZ(boneLabel)
IBL(partialBoneLabel)      ==   IndirectBoneLabel(partialBoneLabel)
GBD(boneLabel1,boneLabel2) ==   getBoneDistance(boneLabel1,boneLabel2)
GBEX(boneLabel)            ==   getBoneExtension(boneLabel)
GBFX(boneLabel)            ==   getBoneFlex(boneLabel) 

Using these abbreviations on the HandWeb() declaration from section 3.2 results in 
this much shorter version:         

#macro HandWeb () 
  blob{ threshold 0.60                                   
    cylinder{GBE(IBL("Thumb_Palm")),GBM(IBL("Index_Palm")),0.25,1}          
    cylinder{GBE(IBL("Index_Palm")),GBM(IBL("Thumb_Palm")),0.25,1}          
    cylinder{GBS(IBL("Thumb_Palm")),GPOB(IBL("Index_Palm"),0.75),0.25,1}
    texture{skinTex}
  }
#end                
            
            
        
            
//////////////  3.4 skeleton information macros 

The skeleton information macros are fairly straightforward:
            
        getSkeletonRotation (SkelName)
// returns a rotation vector in POVray's coordinate system
                                                                   
                                   
        getSkeletonPosition (SkelName)
// returns an XYZ vector representing the location of the skeleton's Zero bone                                
                                
                                
        getSkeletonScale(SkelName)
// returns the scale factor of the skeleton




///////////////////////////////////////////////////////////////////////////////////

            
            

//////////////       

//////////////  4.1 rotating bones

Bones can be rotated about their starting points along three axes: pitch, yaw, and
roll.  The bone first starts with the same orientation as its Parent bone.  Then the
pitch and yaw are changed according to the bone's Pitch and Yaw values, changing the
bone's direction and Ending point.  Then the Roll is applied around the new 
direction vector, resulting in a new coordinate system for the bone.

There are three different ways of rotating bones about their starting point.  First,
the rotation vector can be set, either when the bone is added to a skeleton or using
the setBoneRotation macro:

        setBoneRotation(boneLabel, PYR) 
// rotation relative to bone's parent
// the PYR is the <Pitch,Yaw,Roll> vector
// all angles are expressed in degrees


The second way to rotate bones is to move them relative to their current position:

        rotateBone(boneLabel,deltaPYR) 
// rotates bone relative to current orientation
// deltaPYR is the change in the Pitch, Roll and Yaw
// all angles are expressed in degrees 


Third, a bone may be "flexed" or "extended".  If a bone is fully flexed, then its 
Pitch/Yaw/Roll values equal the MinRot values.  If a bone is fully extended, then 
its Pitch/Yaw/Roll values are equal to its MaxRot values.  The flex or extend may be 
applied to all three of the bone's rotation values at once, or individually; for
instance, one can flex the bone Pitch without affecting the Yaw or Roll.  The amount
of flexion or extension is expressed as a Fraction value such that 

  0 <= Fraction <= 1.  
  
A flex of 1 is equal to an extend of 0, a flex of 0.75 is equal to an extend of 
0.25, and so on.  This is a much easier way to deal with rotating bones than 
rotating by some degree amount, as it is easier to think of what percentage a bone
is flexed or extended within its limits than it is to figure out exact degrees and
their sign.  The macros that flex and extend bones are:


        flexBonePitch(boneLabel, Fraction) 
// bone Pitch set at some Fraction between MaxRot.x (0) and MinRot.x (1)  


        flexBoneYaw(boneLabel, Fraction)
// bone Yaw set at some Fraction between MaxRot.y (0) and MinRot.y (1)


        flexBoneRoll(boneLabel, Fraction) 
// bone Roll set at some Fraction between MaxRot.z (0) and MinRot.z (1)


        flexBone(boneLabel, PYRFractions) 
// bone rotations are set at some Fraction between MaxRot (0) and MinRot (1)
// PYRFractions is a 3d vector, with all three values between 0 and 1 inclusive


        extendBonePitch(boneLabel, Fraction)
// bone Pitch set at some Fraction between MinRot.x (0) and MaxRot.x (1)


        extendBoneYaw(boneLabel, Fraction)
// bone Yaw set at some Fraction between MinRot.y (0) and MaxRot.y (1)


        extendBoneRoll(boneLabel, Fraction)
// bone Roll set at some Fraction between MinRot.z (0) and MaxRot.z (1)


        extendBone(boneLabel, PYRFractions) 
// bone rotations are set at some Fraction between MinRot (0) and MaxRot (1)
// PYRFractions is a 3d vector, with all three values between 0 and 1 inclusive
                                                                

The flex and extend macros make it easier to create the Poses required for the
interpolateMovement macro (see section 4.6).        




//////////////  4.2 bone rotation limits  

It can be useful to limit the range of rotation of a bone.  For instance, if a bone
represents a finger bone, locking the Roll value is a good idea, as finger bones 
don't generally spin axially at the joint unless some horrible injury has been
suffered.  Likewise, the rotation of a bone can be limited to some range.  These
limits - MinRot and MaxRot - are 3-dimensional vectors which correspond to the 
Pitch/Yaw/Roll vector:

<MinPitch,MinYaw,MinRoll>  <=  <Pitch,Yaw,Roll>  <=  <MaxPitch,MaxYaw,MaxRoll>

Attempts to rotate a bone past its MinRot or MaxRot values will stop the bone at
those limits.  Attempts to set a bone's <Pitch,Yaw,Roll> values outside the MinRot 
to MaxRot range result in a "Rotation out of range" error.

A bone may rotate freely along all three axes if 
     MinRot = RotMinInf = <-361,-361,-361>
 and MaxRot = RotMaxInf = <361,361,361>.  

A bone may rotate freely along one axis as long as its Min and Max values are 
outside the range -180 <= theta <= 180.  

To set the bone rotation limits, either one can set them as the bone is declared 
(see section 2.2) or with the following macros:
                                                           
                                                           
        limitBoneMin (boneLabel,PYR)
// set the MinRot vector below which the Pitch Yaw and Roll are not allowed                  
                                  

        limitBoneMax (boneLabel,PYR)
// set the MaxRot vector above which the Pitch Yaw and Roll are not allowed         
          
                   
        limitBoneRot (boneLabel,PYRmin,PYRmax)
// set both minimum and maximum limits on pitch yaw and roll


It can be useful to lock the rotation of a bone along one or more axes.  To do this,
bones.inc sets the Min/Max rotation values to equal the current Pitch/Yaw/Roll of
the bone:


        lockBoneRot (boneLabel) 
// do not allow bone to rotate


        lockBonePitch (boneLabel) 
// do not allow bone to pitch
   
   
        lockBoneYaw (boneLabel)  
// do not allow bone to yaw   


        lockBoneRoll (boneLabel)    
// do not allow bone to roll    


When you have finished creating a skeleton, you may have a number of bones that 
should be locked (for instance, most Zero bones).  Rather than looking through the
entire skeleton for bones you may have missed, you can lock all the freely-rotating
bones in the skeleton:


        lockBoneIfFree (boneLabel)
// lock a bone if it presently rotates freely around all axes


        lockFreeBones (SkelName)
// lock all freely-rotating bones in a skeleton

   
So, just as bones may have their rotation locked, their rotations may be unlocked 
as well, either individually or all rotations at once:


        unlockBoneRot (boneLabel) 
// allow bone to rotate freely around all axes                                                                  


        unlockBonePitch (boneLabel)
// allow bone to pitch freely


        unlockBoneYaw (boneLabel)
// allow bone to yaw freely
 
 
        unlockBoneRoll (boneLabel)
// allow bone to roll freely         
 
 
 
          
//////////////  4.3 scaling a bone or skeleton  

The length of a bone can be set when the bone is created (section 2.2) or it may be 
changed at any time later on.  There are three ways to do this.  First, you can set 
the bone length (independent of scaling factors):    


        setBoneLength (boneLabel,boneLength)
// setting length ignoring scaling factors 
                                            
                                            
Secondly, you can set the scale of the bone (and its associated object in three
dimensions), independent of the skeleton scaling factor:  


        setBoneScale(boneLabel,Scale)
// setting scale factor ignoring all previous scalings of this bone 


Finally, you can set scale the bone relative to its current scaling factor 
(including the skeleton scale):       


        scaleBone(boneLabel,Scale)
// scaled relative to all previous scalings of this bone 


        scaleBoneXYZ(boneLabel,scaleXYZ)
// scale 3D object attached to bone without affecting bone scale 
// all components of scaleXYZ must be greater than zero 


Likewise, the skeleton itself can also be scaled in two ways, either by setting the 
scale directly or scaling relative to prior scaling factor.  When a skeleton is 
scaled, all the bones in the skeleton are scaled by the same factor.  


        setSkelScale (SkelName,Scale)             
// set skeleton absolute scale value


        scaleSkeleton (SkelName,Scale)             
// scale skeleton relative to current scale 




//////////////  4.4 rotating a skeleton   

Skeleton rotation is accomplished by rotating the Zero bone of the skeleton.
Skeletons may have their rotation set in one of two ways.  First, a skeleton's
rotation, relative to POVray's coordinate system, may be set directly:


        setSkelRotation (SkelName, PYR)          
// rotate skeleton relative to POVray's coordinate system


Secondly, a skeleton may be rotated relative to its current orientation:


        rotateSkeleton (SkelName,deltaPYR)            
// rotate skeleton relative to current orientation 
     

These rotations are the same type of rotation as experienced by other bones and are
NOT the same as rotations of other objects performed by POVray.  To see the rotation
the way POVray sees it, you need to use the getSkeletonRotation macro (section 3.4).

     
     

//////////////  4.5 moving a skeleton 
                                        
                                        
There are three ways of moving a skeleton around.  First, you can simply set the 
skeleton's position:
                            
                            
        setSkelPosition (SkelName,XYZ)              
// set skeleton's XYZ position relative to POVray's coordinate system origin           
                                          
                                          
Secondly, you can translate the skeleton:


        translateSkeleton (SkelName,deltaXYZ)             
// set skeleton position relative to current position in POV's coordinate system


Finally, you can move the skeleton relative to its current position and current 
orientation:


        moveSkeleton (SkelName,XYZ)                   
// set skeleton position relative to current position and skeleton orientation  

     
     

//////////////  4.6 animating a skeleton: interpolating poses and movements

  
The entire state of a skeleton's parameters is called a Pose.  Once a skeleton is
created and all the bones have POV objects attached, all the rotations and movements
that set a skeleton's parameters contribute to the Pose.  All of the macros that
went into creating that Pose can be collected together in a single macro, like this:


//
#macro StandBody()
  extendBone(IBL("RLeg_Thigh"),<0.7,0.33,0.75>)
  flexBonePitch(IBL("RLeg_Calf"),0.9)
  flexBonePitch(IBL("RLeg_Heel"),1)
  extendBonePitch(IBL("RLeg_Foot"),0.64)   
  setBoneRotation(IBL("RArm_Upperarm"),<0,-75,0>)
  setBoneRotation(IBL("RArm_Forearm"),<-45,0,0>)
  setBoneRotation(IBL("RArm_Hand_Wrist"),<0,0,0>)   
  extendBone(IBL("LLeg_Thigh"),<0.7,0.33,0.75>)
  flexBonePitch(IBL("LLeg_Calf"),0.9)
  flexBonePitch(IBL("LLeg_Heel"),1)
  extendBone(IBL("LLeg_Foot"),<0.64,0,0>)   
  setBoneRotation(IBL("LArm_Upperarm"),<0,75,0>)
  setBoneRotation(IBL("LArm_Forearm"),<-45,0,0>)
  setBoneRotation(IBL("LArm_Hand_Wrist"),<0,0,0>) 
  setBoneRotation(IBL("Neck"),<0,0,0>)
#end
//


The IndirectBoneLabel macro is used here so that a pose is not limited to a single
skeleton; any skeleton having the same bone names mentioned in the Pose macro can
use that pose.

Several Poses can also be combined together in yet another macro to 
produce another pose.  For instance, if there was another Pose called MakeFist,
it could be combined with the above StandBody Pose by writing another Pose macro
which calls both MakeFist and StandBody:


//
#macro StandingWithFistClenched()
  StandBody()
  MakeFist()
#end
//


Combining two poses in this way is no problem as long as the Poses StandBody and 
MakeFist do not both contain references to the same bone.  If they did, then the 
position of that bone would be entirely determined by MakeFist.

What if you have two different Poses that use many of the same bones, and you want
to generate a new Pose about a third of the way from one to the other?  A construct
like StandingWithFistClenched will clearly not suffice.  Instead we use the
interpolatePose macro:


        interpolatePose(SkelName,PoseString0,PoseString1,Fraction)
        
A Fraction value of 0 would return the Pose in PoseString0, a Fraction of 1 would 
return the Pose in PoseString1, and a Fraction value between 0 and 1 produces a new
Pose, a linear interpolation of the Length, Scale, boneScaleXYZ, Pitch, Yaw, and 
Roll of every bone in skeleton SkelName, as well as the Starting/Ending position of
the Zero bone of the skeleton.  If we wanted a new Pose which was 70% of the way 
between the StandBody and MakeFist Poses, we would generate it like this:


//
#macro newStandingFist(SkelName)
  interpolatePose(SkelName,"StandBody()","MakeFist()",0.7)
#end
//


It is VERY IMPORTANT that Pose macros NOT contain any bone addition or deletion
macro calls, nor any skeleton create/copy/join/mirror/delete macro calls.  These 
will renumber Parent bones and the resulting interpolation will have bizarre 
effects.

When animating the skeleton, we are guiding the skeleton through a series of Poses
and interpolating between them to give us a Pose appropriate to the clock value.
The movement is thus a series of Pose macro calls along with their respective 
timestamps.  This is achieved with the interpolateMovement macro:
        
        
        interpolateMovement(SkelName,PoseStringArray,TimestampArray,Fraction)
        
        
The interpolateMovement macro requires the skeleton name, an array of strings
containing calls to the Pose macros (these are parsed only as needed), an equal-
sized array of timestamp float numbers (starting at 0.0 and ending at 1.0, each 
timestamp greater than the previous), and a Fraction value such that

  0 <= Fraction <= 1
  
This macro interpolates a skeleton's position from among a sequence of Poses marked
by a sequence of time stamps between zero (the first Pose) and one (the final Pose).
The interpolation is either a direct Pose copy when a timestamp matches the 
Fraction value, or between two poses when the Fraction value is between two 
timestamps.  The Fraction value can be dependent upon the clock value, however
Fraction must be a value between 0 and 1 - any values outside that range would be 
extrapolation rather than interpolation and will generate an error.

A cyclic movement would have the same Pose as its first and last Poses:


//
#macro WalkBiped(SkelName,Time)
  interpolateMovement(SkelName,
    array[5]{"RFootUp()","RFootForward()","LFootUp()","LFootForward()","RFootUp()"},
    array[5]{0,0.25,0.5,0.75,1},Time)
#end
//


Each of the Poses (RFootUP() etc.) is stored in a string so that it is not called 
immediately, but instead is only parsed if interpolateMovement requires it to be 
parsed.

The interpolation between poses is performed through linear interpolation.  The 
resulting animation is fairly smooth.  It could possibly be smoother and more 
natural if the interpolation used another method like spline interpolation, however
it is probably easier to simply add more intermediate Poses to a movement.

Making a transition from one type of movement to another is the same as creating a
new type of movement - simply have a Pose from each movement as the first and final
Poses of the new movement:


//
#macro StartBipedWalk(SkelName,Time)
  interpolatePose(SkelName,"StandBody()","RFootUp()",Time)
#end
//


As shown in the above example, interpolatePose is really just a special case of 
interpolateMovement with only two Poses in the PoseStringArray.




///////////////////////////////////////////////////////////////////////////////////


                                        
           
//////////////  

//////////////  5.1 visualizing bones and skeletons  
                                                

Sometimes you may want to see just the skeleton, and not the objects attached to
the skeleton.  This would be useful if the objects are complex and have a time-
consuming rendering; while you are setting up your scene, you may not be 
interested in the final "look", but more interested in where your characters are.

To do this, you can show a skeleton represented by defaultBone objects, which
can be set to a range between transparent and opaque:
                                                                      

        showSkeleton(SkelName,Visibility) 
// shows the skeleton with defaultBone objects rather than attached POV objects
// Visibility of the skeleton is 0 (transparent) to 1 (opaque)


If you want to show all skeletons in the scene this way, you don't need to list 
them all.  You just need to declare the visibility of the skeletons:
          
        
        showAllBones(Visibility)
// shows all bones in the scene as defaultBone objects rather than attached POV
// objects.  Visibility of the bones is 0 (transparent) to 1 (opaque)   


            
            
//////////////  5.2 drawing POV objects attached to bones and skeletons
           
There are three ways to draw the objects attached to bones.  First, you can draw the
object attached to a single bone:


        drawBone (boneLabel) 
// shows the POV object attached to the bone           
        
        
Next, you can draw all the objects attached to a single skeleton:


        drawSkeleton (SkelName)
// shows POV objects attached to all bones in a skeleton
        
        
Finally, you can draw all the objects attached to all the bones in the scene:


        drawAllBones()
// shows POV objects attached to all bones created so far        




///////////////////////////////////////////////////////////////////////////////////
        
        
        
            
//////////////  

//////////////  6.1 reserved keywords 


If you don't want to dig deep into bones.inc, then you don't have to read section 6.           


There are several variable names set aside for internal use by bones.inc.  They are:

        ThisBone           // used to indirectly reference a skeleton
  
        ParentLengthScale  // 0 : used to reference the allBones array
        
        boneScaleXYZ       // 1 :              "
  
        PitchYawRoll       // 2 :              "
  
        Starting           // 3 :              "
  
        Ending             // 4 :              "
  
        AbsRot             // 5 :              "
  
        AxisX              // 6 :              "
  
        AxisY              // 7 :              "
  
        AxisZ              // 8 :              "
  
        MinRot             // 9 :              "
  
        MaxRot             // 10 :             " 

        RotMinInf          // <-361,-361,-361>
  
        RotMaxInf          // <361,361,361>
  
        InvisibleBone      // completely transparent unit sphere
      
        defaultBone(V)     // - variable-visibility bonelike POVray object
                           // - V is between 0 (transparent) and 1 (opaque)
  
  
  
                
//////////////  6.2 the bone and skeleton arrays    


There are two arrays which contain all skeleton data, and five arrays which contain
all the bone data.  

The skeleton arrays are:

SkeletonNames[numSkels] - containing name strings, and
SkeletonPtr[numSkels]   - containing pointers to the Zero bone of each skeleton in 
                          the bone arrays.

The bone arrays are:

BoneNames[numBones]     - containing name strings,
BoneUsed[numBones]      - integer, used to isolate a skeleton for various macros
BoneObject[numBones]    - string containing the name of a POV object or macro
BoneInSkelNum[numBones] - pointer to the skeleton array index number
BoneMirrored[numBones]  - Boolean: whether bone has been mirrored (1) or not(0)
allBones[numBones][10]  - 3D vectors containing the bone's:
                          - <Parent, Length, Scale>
                          - <Pitch, Yaw, Roll>
                          - <X,Y,Z> of the start of the bone
                          - <X,Y,Z> of the end of the bone
                          - <thetaX,thetaY,thetaZ> (rotation in POV's format)
                          - <X,Y,Z> unit vector of bone's coordinate X axis
                          - <X,Y,Z> unit vector of bone's coordinate Y axis
                          - <X,Y,Z> unit vector of bone's coordinate Z axis
                          - <MinPitch,MinYaw,MinRoll>
                          - <MaxPitch,MaxYaw,MaxRoll>
                          
                          
                          
                          
//////////////  6.3 diagnostics


While writing bones.inc, it was very helpful to have a set of diagnostic routines
that showed the contents of the bone and skeleton arrays.  These diagnostics are
kept in the final version in case anyone needs them later.
                                                              
                                                              
        debugBone (boneNumber) 
// displays much of a bone's array data in the Messages window


        debugSkeleton (SkelName) 
// displays bone data for all bones of one skeleton in the Messages window


        debugAllBones () 
// displays bone data for all bones in the Messages window


        debugAllSkels() 
// displays skeleton array data for all skeletons in the Messages window


        debugSkelsAndBones()
// displays array data for all skeletons and bones in the Messages window
// this is the one I used the most, modifying debugBone as necessary.




//////////////  6.4 background macros


What follows are a list of all the background routines which allow direct access to
the bone and skeleton arrays, manipulate strings, perform constructor and destructor
operations, and perform the bone and skeleton position and orientation calculations. 


//////////////  6.4.1 array information and array size manipulation macros


        getNumBones()
// returns the number of bones in the bone arrays


        getNumSkels()
// returns the number of skeletons in the skeleton arrays


        clearBoneUsed()
// sets the BoneUsed array to all zeros


        sumBoneUsed()
// returns the sum of all the values in the BoneUsed array


        expandBoneArrays(moreBones)
// adds moreBones more elements to each of the various bone arrays


        expandSkeletonArrays(moreSkels)
// adds moreSkels more elements to the skeleton arrays


        condenseBoneArrays() 
// this macro is basically a "destructor" of unused bone objects.
// prior to the call to this macro, any bones to be deleted will have been marked
// by having their Parent value changed to -1. 


        condenseSkelArrays()
// "destructor" of unused skeleton objects.  Prior to the call to this macro,
// any skeletons to be deleted will have a SkeletonPtr value of -1.


        BonesDeletion()
// "destructor" of bones and skeletons.  any bone marked with a BoneUsed value of 1
// is kaput.  This macro is called by the macros that create, copy, join, and delete
// skeletons and bones and ensures proper garbage collection.


        makeSkelCopy (SkelName,newSkelNum)
// this is called by copySkeleton and joinSkeletons and does the actual copying
// copySkeletons will keep the old copy, and joinSkeletons gets rid of the old one  


        copyBonesToTemp(BonePtr,BoneVals,SkelStart,TempArrNum)
// used by the interpolatePose macro
// copies allBones data into one of several temporary arrays 


        copyTempToBones(BonePtr,BoneVals,SkelStart,TempArrNum)
// used by the interpolatePose macro
// copies data from one of several temporary arrays into allBones

   
   
   
//////////////  6.4.2 bone and skeleton name string information and manipulation 
//     also marking of skeletons (or just a bone and its children) for later use 


        LookupSkelNum(SkelName)
// attempt to find SkelName in the SkeletonNames array
// return the index to that array if found or -1 if not found


        getSkelNum(SkelName) 
// get the index to skeleton arrays corresponding to this skeleton name


        checkBoneOrSkelName(Name)
// make sure new names are valid


        checkSkelName(SkelName) 
// make sure new skeleton name is valid and is not already in use


        getBoneZero(SkelName) 
// find the Zero bone for a given skeleton


        markBones(startBone)  
// sets BoneUsed to 1 for the startBone and all child bones of startBone


        markSkeleton(SkelName)
// sets BoneUsed to 1 for all bones in a given skeleton


        getFirstUnderscore(boneLabel)
// finds the location of the first underscore in a bone label


        getSkelBoneNames(boneLabel)
// separates a bone Label into skeleton name and bone name


        LookupBoneNumber(boneLabel)
// tries to find a bone index number from the bone label, -1 if not found


        getBoneNumber(boneLabel) 
// - return the bone index number for a given bone label
// - probably the most-used macro for translating from user-side macros to
//   background macros


        checkBoneName(SkelName,newName) 
// make sure new bone name is valid and not already in use


        markBoneChildren (boneLabel)
// set BoneUsed to 1 for a bone and all its children


        findBoneLabel(boneNum)
// - returns the bone label for a given bone number
// - inverse operation of getBoneNumber


        getFinalUnderscore(boneLabel)  
// returns the location of the last underscore in a bone label


        getBonePrefix(boneLabel)
// returns everything before the final underscore in a bone's label


        findBonePrefix(boneNum)
// calls getBonePrefix and findBoneLabel


        getBoneSuffix(boneLabel)
// returns everything after the final underscore in a bone's label


        findBoneSuffix(boneNum)
// calls getBoneSuffix and findBoneLabel


        Parse_String(String)
// Parse_String is copied here from strings.inc for faster compiling


//////////////  6.4.3 internal bone information macros 


        findBoneRot (boneNum)            
// rotation relative to povray's reference frame, using povray's rotation vectors


        findBoneRelRot (boneNum)                   
// rotation relative to parent bone


        findBoneExtension(boneNum)
// returns a 3d vector with the fractional extension values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll  


        findBoneFlex(boneNum)
// returns a 3d vector with the fractional flexion values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll


        findBoneStart(boneNum)
// returns the XYZ coordinates of the starting point of the bone


        findBoneEnd(boneNum) 
// returns the XYZ coordinates of the ending point of the bone


        findBoneMidpoint(boneNum)
// returns the XYZ coordinates of the midpoint of the bone


        findBoneAxisX(boneNum)
// returns the 3d unit vector representing the X axis 
//   in the bone's coordinate system


        findBoneAxisY(boneNum) 
// returns the 3d unit vector representing the Y axis 
//   in the bone's coordinate system  


        findBoneAxisZ(boneNum)
// returns the 3d unit vector representing the Z axis 
//   in the bone's coordinate system 


        findBoneZero(boneNum)
// returns the index of the Zero bone of the skeleton containing bone # boneNum


        findSkelNum(boneNum)
// returns the skeleton number for a given bone


        findSkelScale(boneNum)
// returns the scale of a skeleton given the number of a bone in the skeleton


        findBoneLength(boneNum)
// finds the length of a bone after all scalings


        addBoneRot (OldTheta,Delta,MinTheta,MaxTheta)
// always returns a value between -180 and 180 
// if OldTheta and Delta are in that range


        boneRotate (boneNum,deltaPYR)
// rotates bone by deltaPitchYawRoll within the MinRot and MaxRot limits


//////////////  6.4.4 bone and skeleton calculation macros
 
 
        getAbsRot(Xaxis,Zaxis)
// returns the rotation around POVray's x, y, and z axes


        rotateFrame (oldX,oldY,oldZ,PYR)
// returns new AxisX, AxisY, and AxisZ 3d orthogonal unit vectors based on
// the Parent's coordinate system and the PitchYawRoll of the bone


        calcBone(boneNum)
// determines the starting point, ending point, its own XYZ coordinate axes, and 
// the rotation of a bone as expressed in POVray's rotation system 


        SkeletonCalc(boneNum)
// calculates position and orientation parameters for all child bones of a given
//   bone index number


        calcSkeleton(SkelName) 
// calculates position and orientation parameters for all bones in a skeleton


        calcAllBones()
// calculate the position and orientation of all bones in a scene


        showBoneOnly(boneNum,Visibility) 
// - shows the bone as a defaultBone object rather than the attached POV object
// - Visibility of the bone is 0 (transparent) to 1 (opaque)


        showBoneObj(boneNum)
// - parses the string containing the POV object attached to the bone and shows
//   the corresponding attached object
// - called by drawBone, drawSkeleton, or drawAllBones
 
              
              
              
//////////////  6.5 future expansion and bug reports
              

I'm pretty happy with bones.inc but there is always room for improvement.  I would
like to see some true inverse kinematics or motion capture extensions to bones.inc
to make it easier to generate Poses.  If you want to take a crack at it then feel
free.

Email any bug reports / fixes / enhancements to ed.robotguy@gmail.com; be sure to
include the words "bones.inc" somewhere in the subject line.




///////////////////////////////////////////////////////////////////////////////////

       


//////////////

//////////////  7.1 Index                                                                                   


All the variable and macro names used by bones.inc are listed below alphabetically.
To the left of each is the section number above which details that name.

6.1   AbsRot           
2.2   addBone (ParentLabel, BoneName, PYR, BoneLen, BoneObj)
6.4.3 addBoneRot (OldTheta, Delta, MinTheta, MaxTheta)
2.2   addFullBone (ParentLabel, BoneName, PYR, BoneLen, BoneObj, MinPYR, MaxPYR)
6.2   allBones[numBones][]
2.6   attachObject (boneLabel, ObjectNameString)
6.1   AxisX           
6.1   AxisY          
6.1   AxisZ          
6.2   BoneInSkelNum[numBones] 
6.2   BoneMirrored[numBones]
6.2   BoneNames[numBones]
6.2   BoneObject[numBones]
6.4.3 boneRotate (boneNum, deltaPYR)
6.1   boneScaleXYZ
6.4.1 BonesDeletion()
6.2   BoneUsed[numBones]
6.4.4 calcAllBones()
6.4.4 calcBone (boneNum)
6.4.4 calcSkeleton (SkelName) 
6.4.2 checkBoneName (SkelName, newName)
6.4.2 checkBoneOrSkelName (Name)
6.4.2 checkSkelName (SkelName)
6.4.1 clearBoneUsed()
6.4.1 condenseBoneArrays()
6.4.1 condenseSkelArrays()
6.4.1 copyBonesToTemp (BonePtr, BoneVals, SkelStart, TempArrNum)
2.3   copySkeleton (oldSkelName,newSkelName) 
6.4.1 copyTempToBones (BonePtr, BoneVals, SkelStart, TempArrNum)
2.1   createSkeleton (SkelName)
6.3   debugAllBones ()
6.3   debugAllSkels()
6.3   debugBone (boneNumber)
6.3   debugSkeleton (SkelName)
6.3   debugSkelsAndBones()
6.1   defaultBone (V)
2.5   deleteBone (boneLabel)
2.5   deleteSkeleton (SkelName)
2.6   detachObject (boneLabel)
5.2   drawAllBones()
5.2   drawBone (boneLabel)
5.2   drawSkeleton (SkelName)
6.1   Ending           
6.4.1 expandBoneArrays (moreBones)
6.4.1 expandSkeletonArrays (moreSkels)
4.1   extendBone (boneLabel, PYRFractions)
4.1   extendBonePitch (boneLabel, Fraction)
4.1   extendBoneRoll (boneLabel, Fraction)
4.1   extendBoneYaw (boneLabel, Fraction)
6.4.3 findBoneAxisX (boneNum)
6.4.3 findBoneAxisY (boneNum)
6.4.3 findBoneAxisZ (boneNum)
6.4.3 findBoneEnd (boneNum) 
6.4.3 findBoneExtension (boneNum)
6.4.3 findBoneFlex (boneNum)
6.4.2 findBoneLabel (boneNum)
6.4.3 findBoneLength (boneNum)
6.4.3 findBoneMidpoint (boneNum)
6.4.2 findBonePrefix (boneNum)
6.4.3 findBoneRelRot (boneNum)
6.4.3 findBoneRot (boneNum) 
6.4.3 findBoneStart (boneNum)
6.4.2 findBoneSuffix (boneNum)
6.4.3 findBoneZero (boneNum)
6.4.3 findSkelNum (boneNum)
6.4.3 findSkelScale (boneNum)
4.1   flexBone (boneLabel, PYRFractions)
4.1   flexBonePitch (boneLabel, Fraction)
4.1   flexBoneRoll (boneLabel, Fraction)
4.1   flexBoneYaw (boneLabel, Fraction)
3.3   GBD (boneLabel1, boneLabel2)
3.3   GBE (boneLabel)
3.3   GBEX (boneLabel)
3.3   GBFX (boneLabel)           
3.3   GBL (boneLabel)         
3.3   GBLU (boneLabel)           
3.3   GBM (boneLabel)            
3.3   GBR (boneLabel)         
3.3   GBRR (boneLabel)         
3.3   GBS (boneLabel)            
3.3   GBSC (boneLabel)          
3.3   GBX (boneLabel)             
3.3   GBY (boneLabel)             
3.3   GBZ (boneLabel)             
6.4.4 getAbsRot (Xaxis, Zaxis)
3.1   getBoneAxisX (boneLabel)
3.1   getBoneAxisY (boneLabel)
3.1   getBoneAxisZ (boneLabel)
3.1   getBoneDistance (boneLabel1, boneLabel2)
3.1   getBoneEnd (boneLabel) 
3.1   getBoneExtension (boneLabel)
3.1   getBoneFlex (boneLabel)
3.1   getBoneLength (boneLabel)
3.1   getBoneLengthUnscaled (boneLabel)
3.1   getBoneMaxRot (boneLabel)
3.1   getBoneMidpoint (boneLabel)
3.1   getBoneMinRot (boneLabel)
6.4.2 getBoneNumber (boneLabel)
6.4.2 getBonePrefix (boneLabel)
3.1   getBoneRelRot (boneLabel)
3.1   getBoneRot (boneLabel)
3.1   getBoneScale (boneLabel)
3.1   getBoneScaleXYZ (boneLabel)
3.1   getBoneStart (boneLabel)
6.4.2 getBoneSuffix (boneLabel)
6.4.2 getBoneZero (SkelName)
6.4.2 getFinalUnderscore (boneLabel) 
6.4.2 getFirstUnderscore (boneLabel)
6.4.1 getNumBones()  
6.4.1 getNumSkels()
3.1   getPointOnBone (boneLabel, Fraction)
6.4.2 getSkelBoneNames (boneLabel)
3.4   getSkeletonPosition (SkelName)
3.4   getSkeletonRotation (SkelName)
3.4   getSkeletonScale (SkelName)
6.4.2 getSkelNum (SkelName)
3.3   GPOB (boneLabel, Fraction)  
3.3   IBL (partialBoneLabel)     
3.2   IndirectBoneLabel (partialBoneLabel)
4.6   interpolateMovement (SkelName, PoseStringArray, TimestampArray, Fraction)
4.6   interpolatePose (SkelName, PoseString0, PoseString1, Fraction)
6.1   InvisibleBone  
2.3   joinSkeletons (ChildSkelName, ParentBoneLabel)
4.2   limitBoneMax (boneLabel, PYR)
4.2   limitBoneMin (boneLabel, PYR)
4.2   limitBoneRot (boneLabel, PYRmin, PYRmax)
4.2   lockBoneIfFree (boneLabel)
4.2   lockBonePitch (boneLabel)
4.2   lockBoneRoll (boneLabel)
4.2   lockBoneRot (boneLabel)
4.2   lockBoneYaw (boneLabel)   
4.2   lockFreeBones (SkelName)
6.4.2 LookupBoneNumber (boneLabel)
6.4.2 LookupSkelNum (SkelName)
6.4.1 makeSkelCopy (SkelName, newSkelNum)
6.4.2 markBoneChildren (boneLabel)
6.4.2 markBones (startBone)
6.4.2 markSkeleton (SkelName)
6.1   MaxRot        
6.1   MinRot         
2.3   mirrorSkeleton (SkelName)
4.5   moveSkeleton (SkelName,XYZ)
6.1   ParentLengthScale 
6.4.2 Parse_String (String)
6.1   PitchYawRoll      
2.2   QuickBone (ParentLabel, BoneName)
2.4   renameBone (boneLabel, newName)
2.4   renameSkeleton (oldName, newName)
4.1   rotateBone (boneLabel, deltaPYR)
6.4.4 rotateFrame (oldX, oldY, oldZ, PYR)
4.4   rotateSkeleton (SkelName, deltaPYR)
6.1   RotMaxInf    
6.1   RotMinInf    
4.3   scaleBone (boneLabel, Scale)
4.3   scaleBoneXYZ (boneLabel, scaleXYZ)
4.3   scaleSkeleton (SkelName, Scale)
4.3   setBoneLength (boneLabel, boneLength)
4.1   setBoneRotation (boneLabel, PYR)
4.3   setBoneScale (boneLabel, Scale)
4.5   setSkelPosition (SkelName, XYZ)
4.4   setSkelRotation (SkelName, PYR)
4.3   setSkelScale (SkelName,Scale)
5.1   showAllBones (Visibility)
6.4.4 showBoneObj (boneNum)
6.4.4 showBoneOnly (boneNum,Visibility)
5.1   showSkeleton (SkelName,Visibility)
6.4.4 SkeletonCalc (boneNum)
6.2   SkeletonNames[numSkels]
6.2   SkeletonPtr[numSkels]
6.1   Starting          
6.4.1 sumBoneUsed()
6.1   ThisBone       
4.5   translateSkeleton (SkelName,deltaXYZ)
4.2   unlockBonePitch (boneLabel)
4.2   unlockBoneRoll (boneLabel) 
4.2   unlockBoneRot (boneLabel)
4.2   unlockBoneYaw (boneLabel)
1.3   Zero bone
1.3   Zero skeleton
                
                


//////////////  end of bonesREADME.txt
                                                                                    
