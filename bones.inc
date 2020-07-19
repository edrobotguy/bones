///////////////////////////////////////////////////////////////////////////////////
//    bones.inc
//  
//    by Ed Minchau
//    magicalrobotics.com
//    

/*
version 1.01a for POV-Ray 3.5 or higher
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
 
*/
/////////////////////////////////////////////////////////////////////////////////// 
                             
#include "shapes.inc" 
#include "math.inc"

///////////////////////////////////////////////////////////////////////////////////
//    constants
///////////////////////////////////////////////////////////////////////////////////

#declare ThisBone = 0;       // used to indirectly reference a skeleton

#declare ParentLengthScale = 0;
#declare boneScaleXYZ = 1;
#declare PitchYawRoll = 2;
#declare Starting = 3;
#declare Ending = 4;
#declare AbsRot = 5;
#declare AxisX = 6;
#declare AxisY = 7;
#declare AxisZ = 8;
#declare MinRot = 9;
#declare MaxRot = 10;


#declare RotMinInf = <-361,-361,-361>;
#declare RotMaxInf = -RotMinInf;
                                 
#declare InvisibleBone = 
  sphere{<0,0,0>,0.01 texture{pigment{color rgbft<1,1,1,0,1>}}}

#macro defaultBone(V) 
// V is a visibility value between 0 (transparent) and 1 (opaque)
  merge{
    sphere{<0,0,0>,0.2 texture{pigment{color rgbft<0.5,0.5,0.5,V,1-V>}}}
    cylinder{<0,0,0>,<0,0,0.5>,0.125 texture{pigment{
      color rgbft<0.5,0.5,0.5,V,1-V>}}}
    cone{<0,0,0.5>,0.125 <0,0,1>,0 texture{pigment{color rgbft<0,0,1,V,1-V>}}}
    cylinder{<0,0,0>,<0.2,0,0>,0.125 texture{pigment{color rgbft<1,0,0,V,1-V>}}}
    cylinder{<0,0,0>,<0,0.2,0>,0.125 texture{pigment{color rgbft<0,1,0,V,1-V>}}}
  }
#end
      
      
///////////////////////////////////////////////////////////////////////////////////
//  array declarations
///////////////////////////////////////////////////////////////////////////////////
 
#declare SkeletonNames = array[1]{"Zero"};
#declare SkeletonPtr = array[1]{0};

#declare allBones = array[1][11]{{<0,0,1>,<1,1,1>,<0,0,0>,<0,0,0>,<0,0,0>,<0,0,0>,
  <1,0,0>,<0,1,0>,<0,0,1>,RotMinInf,RotMaxInf}};
#declare BoneNames = array[1]{"Zero"};
#declare BoneUsed = array[1]{0.0};
#declare BoneObject = array[1]{"InvisibleBone"};
#declare BoneInSkelNum = array[1]{0};
#declare BoneMirrored = array[1]{0};
 
  
///////////////////////////////////////////////////////////////////////////////////
//   array information and array size manipulation macros
///////////////////////////////////////////////////////////////////////////////////



#macro getNumBones()
// returns the number of bones in the bone arrays
  dimension_size(allBones,1)
#end  
          
          
  
#macro getNumSkels()
// returns the number of skeletons in the skeleton arrays
  dimension_size(SkeletonNames,1)
#end
          
          
  
#macro clearBoneUsed()
// sets the BoneUsed array to all zeros
  #local NumBones = getNumBones();
  #local Index = 0;
  #while (Index < NumBones)
    #declare BoneUsed[Index]=0;
    #declare Index = Index + 1;
  #end
#end

 
 
#macro sumBoneUsed()
// returns the sum of all the values in the BoneUsed array
  #local Total = 0;
  #local Index = 0;
  #local numBones = getNumBones();
  #while (Index<numBones)
    #declare Total = Total + BoneUsed[Index];
    #declare Index = Index + 1;
  #end
  Total
#end    
              
              

#macro expandBoneArrays(moreBones)
// adds moreBones more elements to each of the various bone arrays
  #local numBones = getNumBones();  
  #local tempAllBones = array[numBones + moreBones][11];
  #local tempBoneNames = array[numBones + moreBones];
  #local tempBISN = array[numBones + moreBones];
  #local tempBoneUsed = array[numBones + moreBones];
  #local tempBoneObject = array[numBones + moreBones];
  #local tempMirrored = array[numBones + moreBones];
  #local Index = 0;
  #while (Index < numBones)
    #declare tempBoneNames[Index] = BoneNames[Index];
    #declare tempBISN[Index] = BoneInSkelNum[Index];
    #declare tempBoneObject[Index] = BoneObject[Index];
    #declare tempBoneUsed[Index] = BoneUsed[Index];
    #declare tempMirrored[Index] = BoneMirrored[Index];
    #local Index2 = 0;
    #while (Index2<11)
      #declare tempAllBones[Index][Index2] = allBones[Index][Index2];
      #declare Index2 = Index2 + 1;
    #end
    #declare Index = Index + 1;
  #end  
  #while (Index < (numBones + moreBones))
    #declare tempBoneNames[Index] = "temp";
    #declare tempBISN[Index] = -1;
    #declare tempBoneObject[Index] = "nullBone";
    #declare tempBoneUsed[Index] = 0;
    #declare tempMirrored[Index] = 0;
    #local Index2 = 0;
    #while (Index2<11)
      #declare tempAllBones[Index][Index2] = <0,0,0>;
      #declare Index2 = Index2 + 1;
    #end
    #declare Index = Index + 1;
  #end  
  #declare allBones = tempAllBones;
  #declare BoneNames = tempBoneNames;
  #declare BoneInSkelNum = tempBISN;
  #declare BoneObject = tempBoneObject;
  #declare BoneUsed = tempBoneUsed;
  #declare BoneMirrored = tempMirrored;
#end        



#macro expandSkeletonArrays(moreSkels)
// adds moreSkels more elements to the skeleton arrays
  #local numSkels = getNumSkels();
  #local tempSkelNames = array [numSkels + moreSkels];
  #local tempSkelPtrs = array [numSkels + moreSkels];
  #local Index = 0;
  #while (Index < numSkels)
    #declare tempSkelNames[Index] = SkeletonNames[Index];
    #declare tempSkelPtrs[Index] = SkeletonPtr[Index];
    #declare Index = Index+1;
  #end
  #declare SkeletonNames = tempSkelNames;
  #declare SkeletonPtr = tempSkelPtrs;
#end       



#macro condenseBoneArrays() 
// this macro is basically a "destructor" of unused bone objects.
// prior to the call to this macro, any bones to be deleted will have been marked
// by having their Parent value changed to -1.  
  clearBoneUsed()
  #local numBones = getNumBones();
  #local NumDelete = 0;
  #local Index = 1;
  #while (Index < numBones)
    #if(allBones[Index][ParentLengthScale].x = -1)  // ie if Bone is to be deleted
      #declare NumDelete = NumDelete + 1;
      #declare BoneUsed[Index] = -1;
    #else
      #declare BoneUsed[Index] = Index - NumDelete;
    #end    
    #declare Index = Index + 1;
  #end 
  #if (NumDelete > 0)
    // the rest of the macro is skipped if there are no bones to delete
    // first, change the pointers in the skeleton array
    #declare Index = 1;
    #local numSkels = getNumSkels();
    #while (Index<numSkels)
      #if (SkeletonPtr[Index] > -1)
        #declare SkeletonPtr[Index]=BoneUsed[SkeletonPtr[Index]];
      #end
      #declare Index = Index + 1;
    #end
    // next create new temporary arrays for bone data
    #local newNumBones = numBones - NumDelete;
    #local tempAllBones = array[newNumBones][11];
    #local tempBoneNames = array[newNumBones];
    #local tempBoneUsed = array[newNumBones];
    #local tempBoneObject = array[newNumBones];
    #local tempBISN = array[newNumBones];  
    #local tempMirrored = array[newNumBones]; 
    // now change the parent pointers to the new values
    #declare Index = 0;
    #declare Index2 = 0;
    #while (Index < numBones)
      #declare PLS = allBones[Index][ParentLengthScale];
      #if (PLS.x>-1)
        #declare tempAllBones[Index2][ParentLengthScale] = 
          <BoneUsed[PLS.x],PLS.y,PLS.z>;
        #declare Index2 = Index2 + 1;
      #end
      #declare Index = Index + 1;
    #end 
     // copy all bone data to temporary arrays
    #declare Index = 0;
    #while (Index < numBones)
      #if(BoneUsed[Index] > -1)
        #local Index3 = 1;
        #while (Index3 < 11)
          #declare tempAllBones[BoneUsed[Index]][Index3] = allBones[Index][Index3];
          #declare Index3 = Index3 + 1;
        #end
        #declare tempBoneNames[BoneUsed[Index]] = BoneNames[Index];
        #declare tempBISN[BoneUsed[Index]] = BoneInSkelNum[Index];
        #declare tempBoneObject[BoneUsed[Index]] = BoneObject[Index];
        #declare tempBoneUsed[BoneUsed[Index]] = BoneUsed[Index];
        #declare tempMirrored[BoneUsed[Index]] = BoneMirrored[Index];
      #end
      #declare Index = Index + 1;
    #end 
    // and redefine the bone arrays as the cleaned up version
    #declare allBones = tempAllBones;
    #declare BoneNames = tempBoneNames;
    #declare BoneInSkelNum = tempBISN;
    #declare BoneObject = tempBoneObject;
    #declare BoneUsed = tempBoneUsed;
    #declare BoneMirrored = tempMirrored;
  #end
  clearBoneUsed()
#end



#macro condenseSkelArrays()
// "destructor" of unused skeleton objects.  Prior to the call to this macro,
// any skeletons to be deleted will have a SkeletonPtr value of -1.
  #local numSkels = getNumSkels();
  #declare tempSkelPtr = array [numSkels];
  #declare tempSkelNames = array [numSkels];
  clearBoneUsed() 
  // first find out how many skeletons to delete
  #local NumDelete = 0;
  #local Index = 0;
  #while (Index < numSkels)
    #if (SkeletonPtr[Index] < 0)
      #declare NumDelete = NumDelete + 1;
    #else
      // no need to delete, so the array information is copied into next 
      // available spot in the temporary array
      #declare tempSkelPtr[Index-NumDelete] = SkeletonPtr[Index];
      #declare tempSkelNames[Index-NumDelete] = SkeletonNames[Index];
      #declare BoneUsed[SkeletonPtr[Index]] = Index - NumDelete;
    #end
    #declare Index = Index + 1;
  #end
  #if (NumDelete>0)
    // the skeleton numbers will change if skeletons are deleted, so we need to
    // change the BoneInSkelNum pointer for each bone
    #declare Index = 1;
    #local numBones = getNumBones();
    #while (Index <numBones)
      #if (BoneUsed[Index]=0)
        #if (allBones[Index][ParentLengthScale].x > -1)
          #declare BoneUsed[Index] = BoneUsed[SkeletonPtr[BoneInSkelNum[Index]]];
        #end
      #end
      #declare Index = Index + 1;
    #end
    #declare Index = 1;
    #while (Index < numBones)
      #declare BoneInSkelNum[Index] = BoneUsed[Index];
      #declare Index = Index + 1;
    #end
    // now we need to copy the temporary arrays to shortened arrays
    #local tempSkelPtr2 = array [numSkels - NumDelete];
    #local tempSkelNames2 = array [numSkels - NumDelete];
    #declare Index = 0;
    #while (Index < (numSkels - NumDelete))
      #declare tempSkelPtr2[Index] = tempSkelPtr[Index];
      #declare tempSkelNames2[Index] = tempSkelNames[Index];
      #declare Index = Index + 1;
    #end 
    // and change the skeleton arrays to our new shortened arrays
    #declare SkeletonNames = tempSkelNames2;
    #declare SkeletonPtr = tempSkelPtr2;
  #end
  clearBoneUsed()
#end



#macro BonesDeletion()
// "destructor" of bones and skeletons.  any bone marked with a BoneUsed value of 1
// is kaput.  This macro is called by the macros that create, copy, join, and delete
// skeletons and bones and ensures proper garbage collection.
  #local numBones = getNumBones();
  #local Index = 0;
  #while (Index<numBones)
    #if (BoneUsed[Index] = 1)
      #local PLS = allBones[Index][ParentLengthScale];
      #declare allBones[Index][ParentLengthScale] = <-1,PLS.y,PLS.z>;
      #declare BoneInSkelNum[Index] = 0;
    #end
    #declare Index = Index + 1; 
  #end
  condenseSkelArrays()
  condenseBoneArrays()
#end



#macro copyBonesToTemp(BonePtr,BoneVals,SkelStart,TempArrNum)
// - used by the interpolatePose macro
// - copies allBones data into one of several temporary arrays
  #local ArrSize = dimension_size(BonePtr,1);
  #local Index = 1;
  #while (Index<ArrSize)
    #local Index2 = 0;
    #while (Index2<3)   
      // copy ParentLengthScale, boneScaleXYZ, and PitchYawRoll parameters
      #declare BoneVals[Index][Index2][TempArrNum]=allBones[BonePtr[Index]][Index2];
      #declare Index2 = Index2 + 1;
    #end
    #declare Index = Index + 1;
  #end
  // copy skeleton Starting parameter
  #declare SkelStart[TempArrNum]=allBones[BonePtr[1]][Starting];
#end 



#macro copyTempToBones(BonePtr,BoneVals,SkelStart,TempArrNum)
// - used by the interpolatePose macro
// - copies data from one of several temporary arrays into allBones
  #local ArrSize = dimension_size(BonePtr,1);
  #local Index = 1;
  #while (Index<ArrSize)
    #local Index2 = 0;
    #while (Index2<3)
      // copy ParentLengthScale, boneScaleXYZ, and PitchYawRoll parameters
      #declare allBones[BonePtr[Index]][Index2]=BoneVals[Index][Index2][TempArrNum];
      #declare Index2 = Index2 + 1;
    #end
    #declare Index = Index + 1;
  #end
  // copy skeleton Starting parameter
  #declare allBones[BonePtr[1]][Starting] = SkelStart[TempArrNum];
#end 

  
         

///////////////////////////////////////////////////////////////////////////////////
//     bone and skeleton name string information and manipulation macros
//     also marking of skeletons (or just a bone and its children) for later use
///////////////////////////////////////////////////////////////////////////////////



#macro LookupSkelNum(SkelName)
// attempt to find SkelName in the SkeletonNames array
// return the index to that array if found or -1 if not found
  #local Index = 0;  
  #local numSkels = getNumSkels();
  #local SkelNum = -1;
  #while (Index < numSkels)
    #if (SkelNum = -1)
      #if (strcmp(SkeletonNames[Index],SkelName)=0)
        #declare SkelNum = Index;
      #end
    #end
    #declare Index = Index + 1;
  #end
  SkelNum
#end



#macro getSkelNum(SkelName) 
// get the index to skeleton arrays corresponding to this skeleton name
  #local SkelNum = LookupSkelNum(SkelName);
  #if (SkelNum = -1)
    #error concat("Skeleton ",SkelName," Not Found\n")
  #end 
  #if (SkelNum = 0) 
    #error "no operations allowed on Zero skeleton"
  #end 
  SkelNum
#end



#macro checkBoneOrSkelName(Name)
// make sure new names are valid
  #local Failed = false;
  #local NameLength = strlen(Name);
  #local Index = 1;
  #while (Index < NameLength + 1)
    #if(!Failed)  
      #local Asc = asc(substr(Name,Index,1));
      #if ((Asc<48)|((Asc>57)&(Asc<65))|((Asc>90)&(Asc<97))|(Asc>122))
        #declare Failed = true;
      #end
    #end
    #declare Index = Index + 1;
  #end
  #if (Failed)
    #error "New Skeleton or Bone names may only use the characters 0..9,a..z,A..Z"    
  #end
  Name
#end



#macro checkSkelName(SkelName) 
// make sure new skeleton name is valid and is not already in use
  #local SkelNum = LookupSkelNum(SkelName); 
  #if (SkelNum > -1)
    #error concat("Skeleton ",SkelName," already exists. \n")
  #end 
  checkBoneOrSkelName(SkelName)
#end
   
    
        
#macro getBoneZero(SkelName) 
// find the Zero bone for a given skeleton
  SkeletonPtr[getSkelNum(SkelName)]               
#end 
                  
         

#macro markBones(startBone)  
// sets BoneUsed to 1 for the startBone and all child bones of startBone
  clearBoneUsed()
  #declare BoneUsed[startBone] = 1;
  #local numBones = getNumBones();
  #local Index = startBone + 1;
  #while (Index < numBones)
    #if(BoneUsed[allBones[Index][ParentLengthScale].x]=1)
      #declare BoneUsed[Index] = 1;
    #end
    #declare Index = Index + 1;
  #end
#end  



#macro markSkeleton(SkelName)
// sets BoneUsed to 1 for all bones in a given skeleton 
  clearBoneUsed()
  #local numBones = getNumBones();
  #local SkelNum = getSkelNum(SkelName);
  #local Index = 1;
  #while (Index<numBones)
    #if (BoneInSkelNum[Index]=SkelNum)
      #declare BoneUsed[Index] = 1;
    #end
    #declare Index = Index + 1;
  #end 
#end   



#macro makeSkelCopy (SkelName,newSkelNum)
// this is called by copySkeleton and joinSkeletons and does the actual copying
// copySkeletons will keep the old copy, and joinSkeletons gets rid of the old one
  #local numBones = getNumBones();
  markSkeleton (SkelName)
  #local moreBones = sumBoneUsed();
  expandBoneArrays (moreBones)
  #local oldSkelIndex = 1;
  #local newSkelIndex = numBones;
  #while (oldSkelIndex < numBones)
    #if (BoneUsed[oldSkelIndex] > 0)
      #declare BoneInSkelNum[newSkelIndex] = newSkelNum;
      #declare BoneUsed[oldSkelIndex] = newSkelIndex;
      #declare BoneNames[newSkelIndex] = BoneNames[oldSkelIndex];
      #declare BoneObject[newSkelIndex] = BoneObject[oldSkelIndex];
      #declare BoneMirrored[newSkelIndex] = BoneMirrored[oldSkelIndex];
      #local Index = 0;
      #while (Index<11)
        #declare allBones[newSkelIndex][Index] = allBones[oldSkelIndex][Index];
        #declare Index = Index + 1;
      #end
      #declare newSkelIndex = newSkelIndex + 1;    
    #end
    #declare oldSkelIndex = oldSkelIndex + 1;
  #end 
  #local Index = numBones; 
  #while (Index < getNumBones())
    #local PLS = allBones[Index][ParentLengthScale];
    #declare allBones[Index][ParentLengthScale] = <BoneUsed[PLS.x],PLS.y,PLS.z>;
    #declare Index = Index + 1;
  #end       
#end
       
       
              
#macro getFirstUnderscore(boneLabel)
// finds the location of the first underscore in a bone label
  #local foundUnderscore = 0;
  #local Index = 1;
  #while (Index<(strlen(boneLabel)+1))
    #if (strcmp(substr(boneLabel,Index,1),"_")=0) 
      #if (foundUnderscore = 0)
        #declare foundUnderscore = Index;                               
      #end
    #end
    #declare Index = Index + 1;
  #end
  #if(foundUnderscore = 0)
    #error concat("no underscore in ",boneLabel,"\n")
  #end 
  foundUnderscore
#end



#macro getSkelBoneNames(boneLabel)
// separates a bone Label into skeleton name and bone name
  #local foundUnderscore = getFirstUnderscore(boneLabel);
  #local SkelName = substr(boneLabel,1,foundUnderscore-1);
  #local BoneName = 
    substr(boneLabel,foundUnderscore+1,strlen(boneLabel)-foundUnderscore);
  array[2]{SkelName,BoneName}
#end       



#macro LookupBoneNumber(boneLabel)
// tries to find a bone index number from the bone label, -1 if not found  
  #local numBones = getNumBones();
  #local SBnames = getSkelBoneNames(boneLabel)
  #local SkelName = SBnames[0];
  #local BoneName = SBnames[1];
  #local Index = getBoneZero(SkelName); 
  markBones(Index)
  #local BoneNumber = -1; 
  #while (Index < numBones)
    #if (BoneNumber = -1)
      #if (BoneUsed[Index]=1)
        #if (strcmp(BoneNames[Index],BoneName)=0)
          #declare BoneNumber = Index;
        #end
      #end
    #end
    #declare Index = Index + 1;
  #end     
  clearBoneUsed()
  BoneNumber
#end 



#macro getBoneNumber(boneLabel) 
// return the bone index number for a given bone label
  #local BoneNumber = LookupBoneNumber(boneLabel);
  #if (BoneNumber = -1)             
    #error concat("Bone ",boneLabel," Not Found\n")                     
  #end
  #if (BoneNumber = 0) 
    #error "no operations allowed on Zero skeleton"
  #end 
  BoneNumber
#end



#macro checkBoneName(SkelName,newName) 
// make sure new bone name is valid and not already in use
  #local boneLabel = concat(SkelName,"_",newName);
  #local BoneNumber = LookupBoneNumber(boneLabel);
  #if (BoneNumber > -1)             
    #error concat("Bone name ",newName," already used in skeleton ",SkelName,"\n")
  #end 
  checkBoneOrSkelName(newName)
#end



#macro markBoneChildren (boneLabel)
// set BoneUsed to 1 for a bone and all its children
  #local startBone = getBoneNumber(boneLabel);
  markBones(startBone)
#end 
    
      
  
#macro findBoneLabel(boneNum)
// returns the bone label for a given bone number
// inverse operation of getBoneNumber
  concat(SkeletonNames[BoneInSkelNum[boneNum]],"_",BoneNames[boneNum])
#end

  

#macro getFinalUnderscore(boneLabel)  
// returns the location of the last underscore in a bone label
  #local foundUnderscore = -1;
  #local Index = strlen (boneLabel);
  #while (Index > 0)
    #if (strcmp(substr(boneLabel,Index,1),"_")=0)
      #if (foundUnderscore = -1)
        #declare foundUnderscore = Index;
      #end
    #end
    #declare Index = Index - 1;
  #end
  #if (foundUnderscore = -1)
    #error concat("no underscore in ",boneLabel)
  #end
  foundUnderscore
#end



#macro getBonePrefix(boneLabel)
// returns everything before the final underscore in a bone's label
  #local foundUnderscore = getFinalUnderscore(boneLabel);
  substr (boneLabel,1,foundUnderscore-1)
#end



#macro findBonePrefix(boneNum)
  getBonePrefix(findBoneLabel(boneNum))
#end



#macro getBoneSuffix(boneLabel)
// returns everything after the final underscore in a bone's label
  #local foundUnderscore = getFinalUnderscore(boneLabel);
  substr (boneLabel,foundUnderscore+1,strlen(boneLabel)-foundUnderscore)
#end
    


#macro findBoneSuffix(boneNum)
  getBoneSuffix(findBoneLabel(boneNum))
#end
    
    

#macro checkFraction(Fraction)
  #if (Fraction<0)
    #error "Fraction must be 0 <= Fraction <= 1  \n"       
  #end
  #if (Fraction>1)
    #error "Fraction must be 0 <= Fraction <= 1  \n"
  #end 
#end 



// Parse_String is copied here from strings.inc for faster compiling
#macro Parse_String(String)
    #fopen FOut "parse_string.tmp" write
    #write(FOut,String)
    #fclose FOut
    #include "parse_string.tmp"
#end

      


///////////////////////////////////////////////////////////////////////////////////
//   internal bone information macros (used by other parts of this file)
///////////////////////////////////////////////////////////////////////////////////



#macro findBoneRot (boneNum)            
// rotation relative to povray's reference frame, using povray's rotation vectors
  allBones[boneNum][AbsRot]
#end

                        
                  
#macro findBoneRelRot (boneNum)                   
// rotation relative to parent bone
  allBones[boneNum][PitchYawRoll]
#end 
      
      

#macro findBoneExtension(boneNum)
// returns a 3d vector with the fractional extension values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local Epsilon = 0.00001;
  // default values for locked bone
  #local FractionP = 0.5;   
  #local FractionY = 0.5; 
  #local FractionR = 0.5;
  // finding fractions 
  #if ((MaxPYR.x-MinPYR.x) > Epsilon)
    #declare FractionP = (PYR.x - MinPYR.x)/(MaxPYR.x - MinPYR.x);
  #end
  #if ((MaxPYR.y-MinPYR.y) > Epsilon)      
    #if (BoneMirrored[boneNum] < 0.5)
      #declare FractionY = (PYR.y - MinPYR.y)/(MaxPYR.y - MinPYR.y);
    #else
      #declare FractionY = (MaxPYR.y - PYR.y)/(MaxPYR.y - MinPYR.y);
    #end
  #end
  #if ((MaxPYR.z-MinPYR.z) > Epsilon)      
    #if (BoneMirrored[boneNum] < 0.5)
      #declare FractionR = (PYR.z - MinPYR.z)/(MaxPYR.z - MinPYR.z);
    #else
      #declare FractionR = (MaxPYR.z - PYR.z)/(MaxPYR.z - MinPYR.z);
    #end
  #end                              
  <FractionP,FractionY,FractionR>
#end
    
    

#macro findBoneFlex (boneNum)
// returns a 3d vector with the fractional flexion values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll
  <1,1,1> - findBoneExtension(boneNum)
#end
 
                  

#macro findBoneStart(boneNum)
// returns the XYZ coordinates of the starting point of the bone
  allBones[boneNum][Starting]
#end
          
            
            
#macro findBoneEnd(boneNum) 
// returns the XYZ coordinates of the ending point of the bone 
  allBones[boneNum][Ending]                         
#end
                                                         
                
        
#macro findBoneMidpoint(boneNum)
// returns the XYZ coordinates of the midpoint of the bone
  (findBoneStart(boneNum)+findBoneEnd(boneNum))/2
#end  


            
#macro findBoneAxisX(boneNum)
// returns the 3d unit vector representing the X axis 
// in the bone's coordinate system
  allBones[boneNum][AxisX]
#end        



#macro findBoneAxisY(boneNum) 
// returns the 3d unit vector representing the Y axis 
// in the bone's coordinate system               
  allBones[boneNum][AxisY]
#end        



#macro findBoneAxisZ(boneNum)
// returns the 3d unit vector representing the Z axis 
// in the bone's coordinate system                
  allBones[boneNum][AxisZ]
#end
 

 
#macro findBoneZero(boneNum)
// returns the index of the Zero bone of the skeleton containing bone # boneNum
  SkeletonPtr[BoneInSkelNum[boneNum]]
#end



#macro findSkelNum(boneNum)
// returns the skeleton number for a given bone
  BoneInSkelNum[boneNum]
#end

  

#macro findSkelScale(boneNum)
// returns the scale of a skeleton given the number of a bone in the skeleton
  allBones[findBoneZero(boneNum)][ParentLengthScale].z
#end

     

#macro findBoneLength(boneNum)
// finds the length of a bone after all scalings
  findSkelScale(boneNum) * allBones[boneNum][ParentLengthScale].y * 
    allBones[boneNum][ParentLengthScale].z
#end    

        
        
#macro addBoneRot (OldTheta,Delta,MinTheta,MaxTheta)
// always returns a value between -180 and 180 
// if OldTheta and Delta are in that range 
  #local Theta = mod(360+ OldTheta + Delta,360);
  #if (Theta>180)
    #declare Theta = Theta - 360;
  #end
  #if (Theta < MinTheta)
    #declare Theta = MinTheta;
  #end
  #if (Theta > MaxTheta)
    #declare Theta = MaxTheta;
  #end
  Theta   
#end



#macro boneRotate (boneNum,deltaPYR)
// rotates bone by deltaPitchYawRoll within the MinRot and MaxRot limits
  #local dPitch = deltaPYR.x;
  #local dYaw = deltaPYR.y;
  #local dRoll = deltaPYR.z;
  #local Pitch = allBones[boneNum][PitchYawRoll].x;
  #local Yaw = allBones[boneNum][PitchYawRoll].y;
  #local Roll = allBones[boneNum][PitchYawRoll].z;
  #local MinPitch = allBones[boneNum][MinRot].x;
  #local MinYaw = allBones[boneNum][MinRot].y;
  #local MinRoll = allBones[boneNum][MinRot].z;
  #local MaxPitch = allBones[boneNum][MaxRot].x;
  #local MaxYaw = allBones[boneNum][MaxRot].y;
  #local MaxRoll = allBones[boneNum][MaxRot].z;    
  #local NewPitch = addBoneRot(Pitch,dPitch,MinPitch,MaxPitch);
  #local NewYaw = addBoneRot(Yaw,dYaw,MinYaw,MaxYaw);
  #local NewRoll = addBoneRot(Roll,dRoll,MinRoll,MaxRoll);
  #declare allBones[boneNum][PitchYawRoll]=<NewPitch,NewYaw,NewRoll>;
#end     
          
        
        
        
///////////////////////////////////////////////////////////////////////////////////
//     bone and skeleton calculation macros
///////////////////////////////////////////////////////////////////////////////////



#macro getAbsRot(Xaxis,Zaxis)
// returns the rotation around POVray's x, y, and z axes  
  #local Pitch = 0;
  #local Roll = 0;
  #local Yaw = -asind(Xaxis.z);
  #if (abs(Xaxis.z)<0.999999)
    #declare Roll = acosd(Xaxis.x/sqrt(pow(Xaxis.x,2)+pow(Xaxis.y,2)));
    #if (Xaxis.y<0)
      #declare Roll = -Roll;
    #end
  #end
  #local tempZ = vaxis_rotate(vaxis_rotate(Zaxis,<0,0,1>,-Roll),<0,1,0>,-Yaw); 
  // take care of possible rounding errors that might make tempZ.z exceed +/- 1
  #if (abs(tempZ.z)>1)
    #if (tempZ.z>1)
      #declare tempZ = <tempZ.x,tempZ.y,1>;
    #else
      #declare tempZ = <tempZ.x,tempZ.y,-1>;
    #end
  #end
  #declare Pitch = -acosd(tempZ.z);
  #if (tempZ.y<0)
      #declare Pitch = -Pitch;
  #end 
  <Pitch,Yaw,Roll>                                    
#end
                                                             


#macro rotateFrame (oldX,oldY,oldZ,PYR)
// returns new AxisX, AxisY, and AxisZ 3d orthogonal unit vectors based on
// the Parent's coordinate system and the PitchYawRoll of the bone
  #local Pitch=PYR.x;
  #local Yaw=PYR.y;
  #local Roll=PYR.z;
  #local tempY = vaxis_rotate (oldY,oldX,Pitch);
  #local tempZ = vaxis_rotate (oldZ,oldX,Pitch);
  #local tempX = vaxis_rotate (oldX,oldY,Yaw);
  #local tempY2 = vaxis_rotate (tempY,oldY,Yaw);
  #local newZ = vaxis_rotate (tempZ,oldY,Yaw);
  #local newY = vaxis_rotate (tempY2,newZ,Roll);
  #local newX = vaxis_rotate (tempX,newZ,Roll);   
  array[3]{newX,newY,newZ}                 
#end     
       
       

#macro calcBone(boneNum)
// determines the starting point, ending point, its own XYZ coordinate axes, and 
// the rotation of a bone as expressed in POVray's rotation system 
  #local parentBone = allBones[boneNum][ParentLengthScale].x;
  #declare allBones[boneNum][Starting]=allBones[parentBone][Ending];
  #local BoneAxes = 
    rotateFrame(allBones[parentBone][AxisX], allBones[parentBone][AxisY],
    allBones[parentBone][AxisZ], allBones[boneNum][PitchYawRoll]);
  #declare allBones[boneNum][AxisX]=BoneAxes[0];
  #declare allBones[boneNum][AxisY]=BoneAxes[1];
  #declare allBones[boneNum][AxisZ]=BoneAxes[2];
  #declare allBones[boneNum][Ending] = 
    allBones[boneNum][Starting]+allBones[boneNum][AxisZ] * findBoneLength(boneNum);
  #declare allBones[boneNum][AbsRot] = 
    getAbsRot(allBones[boneNum][AxisX],allBones[boneNum][AxisZ]);
#end



#macro SkeletonCalc(boneNum)
// calculates position and orientation parameters for all child bones of a given
// bone index number
  markBones(boneNum)
  #local numBones = getNumBones();
  calcBone(boneNum)
  #local Index = boneNum+1;
  #while (Index<numBones)
    #local Parent = allBones[Index][ParentLengthScale].x;
    #if (BoneUsed[Parent]=1)
      calcBone(Index)
    #end
    #declare Index = Index + 1;
  #end
  clearBoneUsed()
#end   
           
   

#macro calcSkeleton(SkelName) 
// calculates position and orientation parameters for all bones in a skeleton
  SkeletonCalc(getBoneZero(SkelName))
#end



#macro calcAllBones()
// calculate the position and orientation of all bones in a scene
  #local Index = 1;
  #local numBones = getNumBones();
  #while (Index < numBones)
    calcBone(Index)
    #declare Index = Index + 1;
  #end
#end 



#macro showBoneOnly(boneNum,Visibility) 
// shows the bone as a defaultBone object rather than the attached POV object
// Visibility of the bone is 0 (transparent) to 1 (opaque)
  #local boneLen = findBoneLength(boneNum);
  #if (boneLen < 0.000001) 
    #declare boneLen = 0.01;
  #end
  object{defaultBone(Visibility) rotate allBones[boneNum][AbsRot] 
        scale boneLen*allBones[boneNum][boneScaleXYZ] 
        translate allBones[boneNum][Starting]}
#end



#macro showBoneObj(boneNum)
// parses the string containing the POV object attached to the bone and shows
// the corresponding attached object
  #declare ThisBone = boneNum; 
  #local POVobject = object{Parse_String("InvisibleBone")}
  #local boneName = BoneNames[boneNum];
  #if (strcmp(findBoneSuffix(boneNum),"Zero")=0)
    #declare POVobject = object {Parse_String(BoneObject[boneNum])}
  #else   
    #local BoneLength = findBoneLength(boneNum);
    #if (BoneLength<0.0000001) 
      #declare BoneLength = 0.0000001;
    #end
    #declare POVobject = object {Parse_String(BoneObject[boneNum])
      scale BoneLength*allBones[boneNum][boneScaleXYZ]
      rotate allBones[boneNum][AbsRot]  
      translate allBones[boneNum][Starting]}
  #end
  markBones(SkeletonPtr[BoneInSkelNum[ThisBone]])
  object{POVobject texture{pigment{color rgbft<1,1,1,0,1>}}}  
#end

     


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//
//  Note: most of the macros before this point are of no interest to the end user 
//  (they are internal macros used by other parts of bones.inc)
//  and the macros after this point are the ones that the end user will use for
//  implementing their own skeletons.  
//
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////
//   external bone information macros  (accessed by end user)    
///////////////////////////////////////////////////////////////////////////////////
                                             


#macro getBoneLength (boneLabel) 
// length of bone after including bone scale and skeleton scale factors
  findBoneLength(getBoneNumber(boneLabel))
#end



#macro getBoneLengthUnscaled (boneLabel)
// raw bone length 
  allBones[getBoneNumber(boneLabel)][ParentLengthScale].y
#end



#macro getBoneScale (boneLabel) 
// bone scaling factor multiplied by skeleton scaling factor
  getBoneLength (boneLabel) / getBoneLengthUnscaled (boneLabel)
#end



#macro getBoneScaleXYZ (boneLabel)
// 3D bone attached object scaling factor
  allBones[getBoneNumber(boneLabel)][boneScaleXYZ]
#end



#macro getBoneRot (boneLabel)            
// rotation relative to povray's reference frame, using povray's rotation vectors
  allBones[getBoneNumber(boneLabel)][AbsRot]
#end

                        
                  
#macro getBoneRelRot (boneLabel)                   
// rotation relative to parent bone
  allBones[getBoneNumber(boneLabel)][PitchYawRoll]
#end 



#macro getBoneExtension (boneLabel)      
// returns a 3d vector with the fractional extension values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll
  findBoneExtension(getBoneNumber(boneLabel))
#end


 
#macro getBoneFlex (boneLabel)  
// returns a 3d vector with the fractional flexion values of the bone between
// the minimum and maximum values of Pitch, Yaw, and Roll
  findBoneFlex(getBoneNumber(boneLabel))
#end



#macro getBoneMinRot (boneLabel)
// returns 3d vector representing the negative limit of pitch, yaw and roll
  allBones[getBoneNumber(boneLabel)][MinRot]
#end       



#macro getBoneMaxRot (boneLabel)
// returns 3d vector representing the positive limit of pitch, yaw and roll
  allBones[getBoneNumber(boneLabel)][MaxRot]
#end
 
                  

#macro getBoneStart(boneLabel)
// returns the POVray XYZ coordinates of the starting point of the bone
  allBones[getBoneNumber(boneLabel)][Starting]
#end
          
            
            
#macro getBoneEnd(boneLabel)     
// returns the POVray XYZ coordinates of the ending point of the bone
  allBones[getBoneNumber(boneLabel)][Ending]
#end
                                                         
                
        
#macro getBoneMidpoint(boneLabel)  
// returns the POVray XYZ coordinates of the midpoint of the bone
  (getBoneStart(boneLabel)+getBoneEnd(boneLabel))/2
#end    



#macro getPointOnBone(boneLabel,Fraction)
// Fraction can be any floating point value
// if Fraction = 0, returns the Starting point of the bone 
// if Fraction = 1, returns the Ending point of the bone
// if Fraction = 0.5, returns the Midpoint of the bone 
// Fraction values greater than 1 return a point outside the bone beyond the Ending
// Fraction values less than 0 return a point outside the bone beyond the Starting
  #local Start = getBoneStart(boneLabel);
  #local End = getBoneEnd(boneLabel);
  Start + (End - Start) * Fraction
#end


            
#macro getBoneAxisX(boneLabel) 
// returns a unit-length vector along the X axis of the bone's coordinate system             
  allBones[getBoneNumber(boneLabel)][AxisX]
#end        



#macro getBoneAxisY(boneLabel) 
// returns a unit-length vector along the Y axis of the bone's coordinate system                
  allBones[getBoneNumber(boneLabel)][AxisY]
#end        



#macro getBoneAxisZ(boneLabel)
// returns a unit-length vector along the Z axis of the bone's coordinate system               
  allBones[getBoneNumber(boneLabel)][AxisZ]
#end
 
 

#macro IndirectBoneLabel(partialBoneLabel)
// - macros which use this function to define objects may only be attached to 
//   bones whose names end in "Zero"    
// - the Zero bone using this macro has already been marked as ThisBone
// - this macro creates a bone label from the Zero bone's label and a partial bone
//   label passed as a parameter
// - an object attached to a Zero bone can refer to any point on any child bone of
//   the Zero bone by using this macro along with getBoneStart, getBoneEnd,
//   getBoneMidpoint, and getPointOnBone, and can find points normal to any child
//   bone of the Zero bone with getBoneAxisX and getBoneAxisY.
  #local ParentName = findBoneLabel(ThisBone);   
  #local newLabel = concat(getBonePrefix(ParentName),"_",partialBoneLabel);
  #if (strcmp(getBoneSuffix(ParentName),"Zero") != 0)
    #error concat("Object strings using the IndirectBoneLabel macro ",
      "may only be attached to bones whose labels end in Zero\n")
  #end
  newLabel
#end     
           
           

#macro getBoneDistance(boneLabel1,boneLabel2) 
// - the getBoneDistance macro is based on the segment to segment distance c++ code
//   found here:  http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
// - returns the distance between the closest two points located between the 
//   Starting and Ending points on two bones (useful for collision detection)
  #local boneNum1 = getBoneNumber(boneLabel1);
  #local boneNum2 = getBoneNumber(boneLabel2);
  #local S1P1 = findBoneStart(boneNum1);  
  #local S1P2 = findBoneEnd(boneNum1);
  #local S2P1 = findBoneStart(boneNum2);  
  #local S2P2 = findBoneEnd(boneNum2);
  #local U = S1P2 - S1P1;
  #local V = S2P2 - S2P1;
  #local W = S2P1 - S1P1;
  #local A = vdot(U,U);     // always >= 0
  #local B = vdot(U,V);
  #local C = vdot(V,V);     // always >= 0
  #local D = vdot(U,W);
  #local E = vdot(V,W);
  #local DD = A*C - B*B;    // always >= 0
  #local SN = 0.0;
  #local SD = DD; 
  #local TN = 0.0;
  #local TD = DD;
  #local Epsilon = 0.00001;
  
  #if (DD < Epsilon)        // lines are almost parallel
    #declare SD = 1.0;
    #declare TN = E;
    #declare TD = C;
  #else                     // get the closest points on the infinite lines
    #declare SN = B*E - C*D;
    #declare TN = A*E - B*D;
    #if (SN < 0)            // if sc < 0 then the s=0 edge is visible
      #declare SN = 0.0;
      #declare TN = E;
      #declare TD = C;
    #else
      #if (SN > SD)         // if sc > 1 then the s=1 edge is visible
        #declare SN = SD;
        #declare TN = E + B;
        #declare TD = C;
      #end
    #end
  #end
  
  #if (TN < 0)              // if tc < 0 then the t=0 edge is visible
    #declare TN = 0.0;
    #if (-D < 0)
      #declare SN = 0.0;
    #else
      #if (-D > A)
        #declare SN = SD;
      #else
        #declare SN = -D;
        #declare SD = A;
      #end
    #end
  #else
    #if (TN > TD)           // if tc > 1 then the t=1 edge is visible
      #declare TN = TD;
      #if (B < D)
        #declare SN = 0.0;
      #else
        #if ((B - D) > A)
          #declare SN = SD;
        #else
          #declare SN = B - D;
          #declare SD = A;
        #end
      #end
    #end
  #end
  
  // finally do the division to get sc and tc      
  #local SC = ((abs(SN) < Epsilon) ? 0.0 : SN/SD);
  #local TC = ((abs(TN) < Epsilon) ? 0.0 : TN/TD);
  
  // get the difference of the two closest points  
  #local DP = W + (SC * U) - (TC * V);
                                                 
  // return the closest distance                                                 
  vlength(DP)  
#end
// end of code copied and modified from http://softsurfer.com



// as some of these macro names are fairly long, here are some abbreviations


#macro GBL(boneLabel) getBoneLength (boneLabel) #end

#macro GBLU(boneLabel) getBoneLengthUnscaled (boneLabel) #end

#macro GBSC(boneLabel) getBoneScale (boneLabel) #end

#macro GBR(boneLabel) getBoneRot(boneLabel) #end
                                          
#macro GBRR(boneLabel) getBoneRelRot(boneLabel) #end

#macro GBS(boneLabel) getBoneStart(boneLabel) #end

#macro GBE(boneLabel) getBoneEnd(boneLabel) #end
        
#macro GBM(boneLabel) getBoneMidpoint(boneLabel) #end     

#macro GPOB(boneLabel,Fraction) getPointOnBone(boneLabel,Fraction) #end 

#macro GBX(boneLabel) getBoneAxisX(boneLabel) #end

#macro GBY(boneLabel) getBoneAxisY(boneLabel) #end

#macro GBZ(boneLabel) getBoneAxisZ(boneLabel) #end    

#macro IBL(partialBoneLabel) IndirectBoneLabel(partialBoneLabel) #end

#macro GBD(boneLabel1,boneLabel2) getBoneDistance(boneLabel1,boneLabel2) #end  

#macro GBEX(boneLabel) getBoneExtension(boneLabel) #end

#macro GBFX(boneLabel) getBoneFlex(boneLabel) #end
           
// end of abbreviations
//




///////////////////////////////////////////////////////////////////////////////////
//     bone manipulation macros   
/////////////////////////////////////////////////////////////////////////////////// 


// rotations                                    

#macro setBoneRotation(boneLabel, PYR) 
// rotation relative to bone's parent
// the PYR is the <Pitch,Yaw,Roll> vector
// all angles are expressed in degrees
  #local boneNum = getBoneNumber(boneLabel);
  #declare allBones[boneNum][PitchYawRoll]=PYR;
// check if the set position is outside the MinRot to Maxrot limits  
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  #if ((MinPYR.x > PYR.x)|(MaxPYR.x < PYR.x))
    #error concat(boneLabel," Pitch out of range\n") 
  #end
  #if ((MinPYR.y > PYR.y)|(MaxPYR.y < PYR.y))
    #error concat(boneLabel," Yaw out of range\n") 
  #end
  #if ((MinPYR.z > PYR.z)|(MaxPYR.z < PYR.z))
    #error concat(boneLabel," Roll out of range\n")                     
  #end
  SkeletonCalc(boneNum)
#end
         
        
         
#macro rotateBone(boneLabel,deltaPYR)
// rotate bone relative to current orientation
// deltaPYR is the change in the Pitch, Roll and Yaw
// all angles are expressed in degrees 
  #local boneNum = getBoneNumber(boneLabel);
  boneRotate (boneNum,deltaPYR)
  SkeletonCalc(boneNum)
#end
          
                    
                    
#macro limitBoneMin (boneLabel,PYR)
// set the MinRot vector below which the Pitch Yaw and Roll are not allowed
  #declare allBones[getBoneNumber(boneLabel)][MinRot] = PYR;
#end
                 
                                  

#macro limitBoneMax (boneLabel,PYR)
// set the MaxRot vector above which the Pitch Yaw and Roll are not allowed
  #declare allBones[getBoneNumber(boneLabel)][MaxRot] = PYR;
#end
    
          
                   
#macro limitBoneRot (boneLabel,PYRmin,PYRmax)
// set both minimum and maximum limits on pitch yaw and roll
  limitBoneMin(boneLabel,PYRmin)
  limitBoneMax(boneLabel,PYRmax)
#end



#macro lockBoneRot (boneLabel) 
// do not allow bone to rotate
  #local PYR = allBones[getBoneNumber(boneLabel)][PitchYawRoll];
  limitBoneRot (boneLabel, PYR, PYR)
#end



#macro lockBonePitch (boneLabel) 
// do not allow bone to pitch
  #local boneNum = getBoneNumber(boneLabel);
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local PYRmin = allBones[boneNum][MinRot];
  #local PYRmax = allBones[boneNum][MaxRot];
  #declare allBones[boneNum][MinRot] = <PYR.x,PYRmin.y,PYRmin.z>;
  #declare allBones[boneNum][MaxRot] = <PYR.x,PYRmax.y,PYRmax.z>;
#end 



#macro lockBoneYaw (boneLabel)  
// do not allow bone to yaw
  #local boneNum = getBoneNumber(boneLabel);
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local PYRmin = allBones[boneNum][MinRot];
  #local PYRmax = allBones[boneNum][MaxRot];
  #declare allBones[boneNum][MinRot] = <PYRmin.x,PYR.y,PYRmin.z>;
  #declare allBones[boneNum][MaxRot] = <PYRmax.x,PYR.y,PYRmax.z>;
#end



#macro lockBoneRoll (boneLabel)    
// do not allow bone to roll
  #local boneNum = getBoneNumber(boneLabel);
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local PYRmin = allBones[boneNum][MinRot];
  #local PYRmax = allBones[boneNum][MaxRot];
  #declare allBones[boneNum][MinRot] = <PYRmin.x,PYRmin.y,PYR.z>;
  #declare allBones[boneNum][MaxRot] = <PYRmax.x,PYRmax.y,PYR.z>;
#end      



#macro unlockBoneRot (boneLabel) 
// allow bone to rotate freely around all axes
  #local boneNum = getBoneNumber(boneLabel);
  #declare allBones[boneNum][MinRot] = RotMinInf;
  #declare allBones[boneNum][MaxRot] = RotMaxInf;
#end  



#macro unlockBonePitch (boneLabel)
// allow bone to pitch freely
  #local boneNum = getBoneNumber(boneLabel);
  #local PYRmin = allBones[boneNum][MinRot];
  #local PYRmax = allBones[boneNum][MaxRot];  
  #declare allBones[boneNum][MinRot] = <-361,PYRmin.y,PYRmin.z>;
  #declare allBones[boneNum][MaxRot] = <361,PYRmax.y,PYRmax.z>;
#end   



#macro unlockBoneYaw (boneLabel)
// allow bone to yaw freely
  #local boneNum = getBoneNumber(boneLabel);
  #local PYRmin = allBones[boneNum][MinRot];
  #local PYRmax = allBones[boneNum][MaxRot];  
  #declare allBones[boneNum][MinRot] = <PYRmin.x,-361,PYRmin.z>;
  #declare allBones[boneNum][MaxRot] = <PYRmax.x,361,PYRmax.z>;
#end   



#macro unlockBoneRoll (boneLabel)
// allow bone to roll freely
  #local boneNum = getBoneNumber(boneLabel);
  #local PYRmin = allBones[boneNum][MinRot];
  #local PYRmax = allBones[boneNum][MaxRot];  
  #declare allBones[boneNum][MinRot] = <PYRmin.x,PYRmin.y,-361>;
  #declare allBones[boneNum][MaxRot] = <PYRmax.x,PYRmax.y,361>;
#end   



#macro lockBoneIfFree (boneLabel)
// lock a bone if it presently rotates freely around all axes
  #if (VEq(getBoneMinRot(boneLabel),RotMinInf))
    #if (VEq(getBoneMaxRot(boneLabel),RotMaxInf))
      lockBoneRot(boneLabel)
    #end
  #end
#end



#macro lockFreeBones (SkelName)
// lock all freely-rotating bones in a skeleton except its Zero bone
  #local numBones = getNumBones();
  markSkeleton(SkelName)
  #local Index = 1;
  #while (Index<numBones)
    #if (BoneUsed[Index] = 1)
      lockBoneIfFree(findBoneLabel(Index))
    #end
    #declare Index = Index + 1;
  #end  
  unlockBoneRot(concat(SkelName,"_Zero"))
  clearBoneUsed()
#end 



#macro flexBonePitch(boneLabel, Fraction) 
// bone Pitch set at some Fraction between MaxRot.x (0) and MinRot.x (1)
  #local boneNum = getBoneNumber(boneLabel);
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  checkFraction(Fraction)
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local newPitch = MaxPYR.x - Fraction * (MaxPYR.x-MinPYR.x);  
  #declare allBones[boneNum][PitchYawRoll] = <newPitch,PYR.y,PYR.z>; 
  SkeletonCalc(boneNum) 
#end



#macro flexBoneYaw(boneLabel, Fraction)
// bone Yaw set at some Fraction between MaxRot.y (0) and MinRot.y (1)
  #local boneNum = getBoneNumber(boneLabel);
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  checkFraction(Fraction)
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local newYaw = 0;
  #if (BoneMirrored[boneNum]<0.5)
    #declare newYaw = MaxPYR.y - Fraction * (MaxPYR.y-MinPYR.y); 
  #else  
    #declare newYaw = MinPYR.y + Fraction * (MaxPYR.y-MinPYR.y); 
  #end
  #declare allBones[boneNum][PitchYawRoll] = <PYR.x,newYaw,PYR.z>; 
  SkeletonCalc(boneNum) 
#end



#macro flexBoneRoll(boneLabel, Fraction) 
// bone Roll set at some Fraction between MaxRot.z (0) and MinRot.z (1)
  #local boneNum = getBoneNumber(boneLabel);
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  checkFraction(Fraction)
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local newRoll = 0;
  #if (BoneMirrored[boneNum]<0.5)
    #declare newRoll = MaxPYR.z - Fraction * (MaxPYR.z-MinPYR.z);  
  #else
    #declare newRoll = MinPYR.z + Fraction * (MaxPYR.z-MinPYR.z);
  #end                                                         
  #declare allBones[boneNum][PitchYawRoll] = <PYR.x,PYR.y,newRoll>; 
  SkeletonCalc(boneNum) 
#end



#macro flexBone(boneLabel, PYRFractions) 
// bone rotations are set at some Fraction between MaxRot (0) and MinRot (1)
// PYRFractions is a 3d vector, with all three values between 0 and 1 inclusive
  flexBonePitch(boneLabel, PYRFractions.x)
  flexBoneYaw(boneLabel, PYRFractions.y)
  flexBoneRoll(boneLabel, PYRFractions.z) 
#end



#macro extendBonePitch(boneLabel, Fraction)
// bone Pitch set at some Fraction between MinRot.x (0) and MaxRot.x (1)
  #local boneNum = getBoneNumber(boneLabel);
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  checkFraction(Fraction)
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local newPitch = MinPYR.x + Fraction * (MaxPYR.x-MinPYR.x);  
  #declare allBones[boneNum][PitchYawRoll] = <newPitch,PYR.y,PYR.z>; 
  SkeletonCalc(boneNum)
#end



#macro extendBoneYaw(boneLabel, Fraction)
// bone Yaw set at some Fraction between MinRot.y (0) and MaxRot.y (1)
  #local boneNum = getBoneNumber(boneLabel);
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  checkFraction(Fraction)
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local newYaw = 0;
  #if (BoneMirrored[boneNum]<0.5)
    #declare newYaw = MinPYR.y + Fraction * (MaxPYR.y-MinPYR.y); 
  #else  
    #declare newYaw = MaxPYR.y -  Fraction * (MaxPYR.y-MinPYR.y); 
  #end                                                     
  #declare allBones[boneNum][PitchYawRoll] = <PYR.x,newYaw,PYR.z>; 
  SkeletonCalc(boneNum) 
#end



#macro extendBoneRoll(boneLabel, Fraction)
// bone Roll set at some Fraction between MinRot.z (0) and MaxRot.z (1)
  #local boneNum = getBoneNumber(boneLabel);
  #local MinPYR = allBones[boneNum][MinRot];
  #local MaxPYR = allBones[boneNum][MaxRot];
  checkFraction(Fraction)
  #local PYR = allBones[boneNum][PitchYawRoll];
  #local newRoll = 0;
  #if (BoneMirrored[boneNum]<0.5)
    #declare newRoll = MinPYR.z + Fraction * (MaxPYR.z-MinPYR.z);  
  #else
    #declare newRoll = MaxPYR.z - Fraction * (MaxPYR.z-MinPYR.z);
  #end
  #declare allBones[boneNum][PitchYawRoll] = <PYR.x,PYR.y,newRoll>; 
  SkeletonCalc(boneNum) 
#end



#macro extendBone(boneLabel, PYRFractions) 
// bone rotations are set at some Fraction between MinRot (0) and MaxRot (1)
// PYRFractions is a 3d vector, with all three values between 0 and 1 inclusive
  extendBonePitch(boneLabel, PYRFractions.x)
  extendBoneYaw(boneLabel, PYRFractions.y)                             
  extendBoneRoll(boneLabel, PYRFractions.z) 
#end   


// length and scale    
  
#macro setBoneLength (boneLabel,boneLen)
// setting length ignoring scaling factors
  #local boneNum = getBoneNumber(boneLabel);
  #local PLS = allBones[boneNum][ParentLengthScale];
  #declare allBones[boneNum][ParentLengthScale] = <PLS.x,boneLen,PLS.z>;
  SkeletonCalc(boneNum)
#end 



#macro setBoneScale(boneLabel,Scale)
// setting scale factor ignoring all previous scalings of this bone
  #local boneNum = getBoneNumber(boneLabel);    
  #local PLS = allBones[boneNum][ParentLengthScale];
  #declare allBones[boneNum][ParentLengthScale] = <PLS.x,PLS.y,Scale>;
  SkeletonCalc(boneNum)
#end                                                    
            
            
            
#macro scaleBone(boneLabel,Scale)
// scaled relative to all previous scalings of this bone
  #local boneNum = getBoneNumber(boneLabel);    
  #local PLS = allBones[boneNum][ParentLengthScale];
  #declare allBones[boneNum][ParentLengthScale] = <PLS.x , PLS.y , PLS.z * Scale>;
  SkeletonCalc(boneNum)
#end 



#macro scaleBoneXYZ(boneLabel,scaleXYZ)
// scale 3D object attached to bone without affecting bone scale 
// all components of scaleXYZ must be greater than zero  
  #local Epsilon = 0.0000001; 
  #local boneNum = getBoneNumber(boneLabel);
  #local Xscale = ((scaleXYZ.x>Epsilon)?scaleXYZ.x:Epsilon);   
  #local Yscale = ((scaleXYZ.y>Epsilon)?scaleXYZ.y:Epsilon);
  #local Zscale = ((scaleXYZ.z>Epsilon)?scaleXYZ.z:Epsilon);  
  #declare allBones[boneNum][boneScaleXYZ] = < Xscale , Yscale , Zscale >;
#end 

                                        
                                        
                                                    
///////////////////////////////////////////////////////////////////////////////////
//   skeleton information macros     
///////////////////////////////////////////////////////////////////////////////////
                                  
                                  
                                                                                                                                                    
#macro getSkeletonRotation (SkelName)
// returns a rotation vector in POVray's coordinate system
  allBones[getBoneZero(SkelName)][AbsRot]
#end
                                  
                                  
                                   
#macro getSkeletonPosition (SkelName)
// returns an XYZ vector representing the location of the skeleton's Zero bone
  allBones[getBoneZero(SkelName)][Ending]
#end  
                                
                                
                                
#macro getSkeletonScale(SkelName)
// returns the scale of the skeleton
  allBones[getBoneZero(SkelName)][ParentLengthScale].z
#end      




///////////////////////////////////////////////////////////////////////////////////
//   skeleton manipulation macros     
///////////////////////////////////////////////////////////////////////////////////
              
              

#macro setSkelRotation (SkelName, PYR)          
// rotate skeleton relative to POVray's coordinate system
  #local boneLabel = concat(SkelName,"_Zero");
  setBoneRotation(boneLabel, PYR)
#end
                                                          
                                                          
                                                          
#macro rotateSkeleton (SkelName,PYR)            
// rotate skeleton relative to current orientation
  #local boneLabel = concat(SkelName,"_Zero");
  rotateBone (boneLabel,PYR)                    
#end

                             
                                 
#macro setSkelScale (SkelName,Scale)             
// set skeleton absolute scale value
  #local boneLabel = concat(SkelName,"_Zero");
  setBoneScale(boneLabel,Scale)
#end                                                              


                                                     
#macro scaleSkeleton (SkelName,Scale)             
// scale skeleton relative to current scale
  #local boneLabel = concat(SkelName,"_Zero");
  scaleBone(boneLabel,Scale)
#end



#macro setSkelPosition (SkelName,XYZ)              
// set skeleton position relative to POVray's coordinate system origin
  #local boneNum = getBoneZero(SkelName);
  #declare allBones[boneNum][Starting] = XYZ;
  #declare allBones[boneNum][Ending] = XYZ;
  SkeletonCalc(boneNum)
#end  
         
         
         
#macro translateSkeleton (SkelName,deltaXYZ)             
// set skeleton position relative to current position in POV's coordinate system
  #local boneNum = getBoneZero(SkelName);
  setSkelPosition(SkelName,allBones[boneNum][Starting] + deltaXYZ)
#end    



#macro moveSkeleton (SkelName,XYZ)                   
// set skeleton position relative to current position and skeleton orientation
  #local boneNum = getBoneZero(SkelName); 
  #local deltaX = XYZ.x * allBones[boneNum][AxisX];
  #local deltaY = XYZ.y * allBones[boneNum][AxisY];      
  #local deltaZ = XYZ.z * allBones[boneNum][AxisZ]; 
  #local Delta = deltaX + deltaY + deltaZ;
  translateSkeleton (SkelName,Delta)
#end      
                               
                               
                               
       
///////////////////////////////////////////////////////////////////////////////////
//   bone and skeleton creation / modification
///////////////////////////////////////////////////////////////////////////////////



#macro attachObject(boneLabel, POVobjectString) 
// POVobjectString is a string containing a regular POVray object 
  #local boneNum = getBoneNumber(boneLabel);
  #declare BoneObject[boneNum] = POVobjectString; 
#end
      


#macro detachObject(boneLabel)
// replace existing object with an invisible sphere  
  #declare BoneObject[getBoneNumber(boneLabel)] = "InvisibleBone";
#end



#macro renameBone(boneLabel,newName)
// give a bone a new name. the new bone's Label will be the skeleton name, followed
// by an underscore, then the new name
  #local boneNum = getBoneNumber(boneLabel);
  #local SkelName = SkeletonNames[BoneInSkelNum[boneNum]];
  #declare BoneNames[boneNum] = checkBoneName(SkelName,newName);
#end



#macro renameSkeleton(oldName,newName)
// surprisingly, this macro gives the skeleton a new name 
  #declare SkeletonNames[getSkelNum(oldName)] = checkSkelName(newName);
#end   



#macro addFullBone( ParentLabel, BoneName, PYR, BoneLen, BoneObj, MinPYR, MaxPYR)
// ParentLabel is the label of the bone that will be the parent of the new bone
// BoneName must be unique within that skeleton
// PYR is the pitch, yaw, and roll of the bone relative to its parent
// BoneLen is the length (scale defaults to 1) independent of skeleton scale
// BoneObj contains a string that will be parsed when the bone is drawn
// MinPYR is a 3d vector with the lower limits of the pitch, yaw, and roll
// MaxPYR is a 3d vector with the upper limits of the pitch, yaw, and roll
  #local boneNum = getNumBones(); 
  #if (strcmp(BoneName,"Zero")=0)
    #local ParentNum = boneNum;
  #else
    #local ParentNum = getBoneNumber (ParentLabel);  
  #end   
  expandBoneArrays(1) 
  #local SBnames = getSkelBoneNames (ParentLabel);
  #local SkelName = SBnames[0];
  #declare BoneInSkelNum[boneNum] = getSkelNum(SkelName);
  #declare BoneNames[boneNum] = checkBoneName(SkelName,BoneName);
  #declare allBones[boneNum][ParentLengthScale] = <ParentNum,BoneLen,1>;
  #declare allBones[boneNum][PitchYawRoll] = PYR;
  #declare allBones[boneNum][MinRot] = MinPYR;
  #declare allBones[boneNum][MaxRot] = MaxPYR;
  #declare allBones[boneNum][boneScaleXYZ] = <1,1,1>;
  #declare BoneObject[boneNum] = BoneObj ; 
  #declare BoneMirrored[boneNum] = 0;                        
  #if (strcmp(BoneName,"Zero")=0)
    #declare allBones[boneNum][AxisX] = <1,0,0>;
    #declare allBones[boneNum][AxisY] = <0,1,0>;
    #declare allBones[boneNum][AxisZ] = <0,0,1>;
    #declare allBones[boneNum][Starting] = <0,0,0>;
    #declare allBones[boneNum][Ending] = <0,0,0>;
    #declare allBones[boneNum][AbsRot] = <0,0,0>;
  #else                                                                                
    calcBone(boneNum)
  #end
#end



#macro addBone( ParentLabel, BoneName, PYR, BoneLen, BoneObj)
// same as addFullBone with no limits on bone rotation 
  addFullBone( ParentLabel, BoneName, PYR, BoneLen, BoneObj, RotMinInf, RotMaxInf)
#end



#macro QuickBone( ParentLabel, BoneName)
// just declaring a new bone, other parameters to be modified later
  addFullBone( ParentLabel,BoneName,<0,0,0>,1,"InvisibleBone", RotMinInf, RotMaxInf)
#end   



#macro deleteBone(boneLabel)                   
// deletes bone and all its children   
  markBones(getBoneNumber(boneLabel))
  BonesDeletion()
#end

 
 
#macro deleteSkeleton(SkelName)
// deletes a skeleton and all its bones  
  markSkeleton(SkelName)
  #declare SkeletonPtr [ getSkelNum(SkelName) ] = -1;
  BonesDeletion()   
#end 



#macro createSkeleton (SkelName)
// creates a new skeleton and its Zero bone
  #local numSkels = getNumSkels(); 
  #local SkeletonName = checkSkelName(SkelName);
  expandSkeletonArrays(1)
  #declare SkeletonNames[numSkels] = SkeletonName;
  #declare SkeletonPtr[numSkels] = getNumBones();
  addBone(concat(SkelName,"_Zero"),"Zero",<0,0,0>,0,"InvisibleBone")
#end
         


#macro copySkeleton(oldSkelName,newSkelName)
// creates copy of an old skeleton with a new name; the old skeleton is retained
  #local numSkels = getNumSkels();
  #local numBones = getNumBones(); 
  #local SkeletonName = checkSkelName(newSkelName);
  expandSkeletonArrays(1)
  #declare SkeletonNames[numSkels] = SkeletonName;
  #declare SkeletonPtr[numSkels] = numBones;
  makeSkelCopy(oldSkelName,numSkels)
  #declare allBones[numBones][Starting]=<0,0,0>;
  #declare allBones[numBones][Ending]=<0,0,0>;
  #declare allBones[numBones][PitchYawRoll]=<0,0,0>;
  SkeletonCalc(numBones)  
#end
                              


#macro mirrorSkeleton(SkelName)
// mirror images the skeleton about the plane normal to the Zero bone's AxisX 
  clearBoneUsed() 
  markSkeleton(SkelName)
  #local Index = 1;
  #local numBones = getNumBones();
  #while (Index<numBones)
    #if (BoneUsed[Index]=1)
      #local PYR = allBones[Index][PitchYawRoll];
      #local MinPYR = allBones[Index][MinRot];
      #local MaxPYR = allBones[Index][MaxRot];
      #declare allBones[Index][PitchYawRoll] = <PYR.x,-PYR.y,-PYR.z>;
      #declare allBones[Index][MinRot] = <MinPYR.x,-MaxPYR.y,-MaxPYR.z>; 
      #declare allBones[Index][MaxRot] = <MaxPYR.x,-MinPYR.y,-MinPYR.z>;
      #declare BoneMirrored[Index] = 1 - BoneMirrored[Index];      
    #end
    #declare Index = Index + 1;
  #end      
  calcSkeleton(SkelName)
#end         
 

 
#macro joinSkeletons(ChildSkelName,ParentBoneLabel)
// connects a skeleton to the end of a bone on another skeleton
// the old skeleton is absorbed into the skeleton to which it is joined
  #local Parent = getBoneNumber (ParentBoneLabel);
  #local ParentSkelNum = BoneInSkelNum[Parent]; 
  #local OldZero = getBoneZero(ChildSkelName);   
  #local SkelScale = allBones[OldZero][ParentLengthScale].z;
  markSkeleton(ChildSkelName)
  #local Index = OldZero ; 
  #local numBones = getNumBones();
  #while (Index < numBones)
    #if (BoneUsed[Index] = 1)
      #declare BoneNames[Index] = concat (ChildSkelName,"_",BoneNames[Index]);
      #local PLS = allBones[Index][ParentLengthScale];
      #declare allBones[Index][ParentLengthScale] = <PLS.x,SkelScale*PLS.y*PLS.z,1>;
    #end
    #declare Index = Index + 1;
  #end
  makeSkelCopy(ChildSkelName,ParentSkelNum)
  #local PLS = allBones[numBones][ParentLengthScale];
  #declare allBones[numBones][ParentLengthScale] = <Parent,PLS.y,PLS.z>;
  deleteSkeleton(ChildSkelName)
  SkeletonCalc(findBoneZero(Parent))
#end



#macro interpolatePose(SkelName,PoseString0,PoseString1,Fraction)
  // linear interpolation between two poses
  #if (strcmp(SkelName,"")=0)
    #declare SkelName = SkeletonNames[BoneInSkelNum[ThisBone]];
  #else
    #declare ThisBone = getBoneZero(SkelName);   
  #end      
  checkFraction(Fraction)  
  
  // creating temporary arrays used to store poses
  markSkeleton(SkelName)
  #local usedBones = sumBoneUsed()+1;
  #local numBones = getNumBones();
  #local BonePtr = array [numBones];
  #local BoneVals = array[numBones][3][4];
  #local SkelStart = array[4];
  #local IndexTemp = 1;
  #local Index = 1;
  
  // selecting bones to pose
  #while ((Index<numBones)&(IndexTemp<usedBones))
    #if (BoneUsed[Index]=1)
      #declare BonePtr[IndexTemp] = Index;
      #declare IndexTemp = IndexTemp + 1;
    #end
    #declare Index = Index + 1;
  #end
  
  // save prior skeleton pose information in temp 0
  copyBonesToTemp(BonePtr,BoneVals,SkelStart,0)              
  
  // generate first pose and store in temp 1
  Parse_String(PoseString0)
  calcSkeleton(SkelName) 
  copyBonesToTemp(BonePtr,BoneVals,SkelStart,1)
  
  // restore prior skeleton pose information
  copyTempToBones(BonePtr,BoneVals,SkelStart,0)  
    
  // generate second pose and store in temp 2 
  Parse_String(PoseString1)    
  calcSkeleton(SkelName)
  copyBonesToTemp(BonePtr,BoneVals,SkelStart,2)
  
  // interpolate between the two poses
  #declare Index = 1;
  #while (Index<numBones)
    #local Index2 = 0;
    #while (Index2<3) 
      #local StartVal = BoneVals[Index][Index2][1];
      #local EndVal = BoneVals[Index][Index2][2];
      #declare BoneVals[Index][Index2][3] = StartVal + (EndVal-StartVal)*Fraction;
      #declare Index2 = Index2 + 1;
    #end
    #declare Index = Index + 1;
  #end
  #local StartVal = SkelStart[1];
  #local EndVal = SkelStart[2];
  #declare SkelStart[3] = StartVal + (EndVal-StartVal)*Fraction;  
  
  // move skeleton into interpolated pose
  copyTempToBones(BonePtr,BoneVals,SkelStart,3)
  calcSkeleton(SkelName)  
#end  




#macro interpolateMovement(SkelName,PoseStringArray,TimestampArray,Fraction)
  // Interpolates a skeleton's position from among a sequence of poses marked by a
  // sequence of time stamps between zero (the first pose) and one (the final pose).
  // The interpolation is either direct when a timestamp matches the Fraction value,
  // or between two poses when the Fraction value is between two timestamps.
  // 0 <= Fraction <= 1
    
  #if (strcmp(SkelName,"")=0)
    #declare SkelName = SkeletonNames[BoneInSkelNum[ThisBone]];
  #else
    #declare ThisBone = getBoneZero(SkelName);   
  #end
  
  // verifying parameters
  #local numPoses = dimension_size(PoseStringArray,1);
  #local numFractions = dimension_size(TimestampArray,1);
  #if (numPoses != numFractions)
    #error concat("\n number of Poses = ",str(numPoses,2,0),
      " ... number of time stamps = ",str(numFractions,2,0),"\n")
  #end
  #if (numPoses < 2)
    #error "\n minimum of two poses required for interpolation\n"
  #end
  #if (TimestampArray[0] != 0)
    #error "\n Timestamp array must start with zero\n"
  #end
  #if (TimestampArray[numFractions-1] != 1)
    #error "\n Timestamp array must end with one\n"
  #end
  #local Index = 1;
  #while (Index<numFractions)
    #if (TimestampArray[Index] <= TimestampArray[Index-1])
      #error "\n Timestamp array must be ascending\n"
    #end
    #declare Index = Index + 1;
  #end
  checkFraction(Fraction)
  // parameter check complete 
       
  // going through time stamps to find 1 equal or 2 bracketing poses 
  #local TimeIndex = 0;
  #local FoundTime = -1;
  #while ((FoundTime<0)&(TimeIndex<numFractions))
    #if(TimestampArray[TimeIndex]<Fraction)
      #declare TimeIndex = TimeIndex + 1;
    #else
      #declare FoundTime = 1;
    #end
  #end 
  
  #if (TimestampArray[TimeIndex] = Fraction)
    // move skeleton into selected pose
    Parse_String(PoseStringArray[TimeIndex])
    calcSkeleton(SkelName)
  #else  
    // interpolate between two bracketing poses
    #local Pose0 = PoseStringArray[TimeIndex-1];
    #local Pose1 = PoseStringArray[TimeIndex];
    #local TimeFrac = (Fraction-TimestampArray[TimeIndex-1]) /
      (TimestampArray[TimeIndex]-TimestampArray[TimeIndex-1]);
    interpolatePose(SkelName,Pose0,Pose1,TimeFrac)
  #end  
#end     
                                              


///////////////////////////////////////////////////////////////////////////////////
//  skeleton visualization macros
///////////////////////////////////////////////////////////////////////////////////
              


#macro showSkeleton(SkelName,Visibility) 
// shows the skeleton with defaultBone objects rather than attached POV objects
// Visibility of the skeleton is 0 (transparent) to 1 (opaque)
  markSkeleton(SkelName)
  #local numBones = getNumBones();
  #local Index = 0;
  #while (Index<numBones)
    #if (BoneUsed[Index]=1)
      showBoneOnly(Index,Visibility)
    #end
    #declare Index = Index + 1;
  #end
  clearBoneUsed()
#end 



#macro showAllBones(Visibility)
// shows all bones in the scene as defaultBone objects rather than attached POV
// objects.  Visibility of the bones is 0 (transparent) to 1 (opaque)   
  #local numBones = getNumBones();
  #local Index = 0;
  #while (Index<numBones)
    showBoneOnly(Index,Visibility)
    #declare Index = Index + 1;
  #end 
#end

  
  
#macro drawBone (boneLabel) 
// shows the POV object attached to the bone
  #local boneNum = getBoneNumber(boneLabel);
  showBoneObj(boneNum)
#end



#macro drawSkeleton (SkelName)
// shows POV objects attached to all bones in a skeleton
  markSkeleton(SkelName)
  #local Index = getBoneZero(SkelName);
  #local numBones = getNumBones();
  #while (Index < numBones)
    #if (BoneUsed[Index] = 1) 
      //#debug concat("\n Drawing ",BoneNames[Index],"\n")
      showBoneObj(Index)
    #end
    #declare Index = Index + 1;
  #end 
  clearBoneUsed()
#end



#macro drawAllBones()
// shows POV objects attached to all bones created so far
  #local Index = 0;
  #local numBones = getNumBones();
  #while (Index < numBones)
    showBoneObj(Index)
    #declare Index = Index + 1;
  #end 
  clearBoneUsed()
#end
  

     

///////////////////////////////////////////////////////////////////////////////////
//           diagnostics
///////////////////////////////////////////////////////////////////////////////////



#macro debugBone (boneNumber) 
// displays much of a bone's array data in the Messages window
  #debug concat(str(boneNumber,2,0)," ",
    SkeletonNames[BoneInSkelNum[boneNumber]]," ") 
  #debug concat(BoneNames[boneNumber]," : ")   
  #debug concat(str(BoneInSkelNum[boneNumber],2,0),",")
  #debug concat(str(BoneUsed[boneNumber],2,0)," ") 
  #local PLS = allBones[boneNumber][ParentLengthScale];
  #debug concat("<",str(PLS.x,2,0),",",str(PLS.y,4,2),",",str(PLS.z,4,2),">")
  #local PYR = allBones[boneNumber][PitchYawRoll];
  #debug concat(" PYR: <",str(PYR.x,2,0),",",str(PYR.y,4,2),",",str(PYR.z,4,2),">") 
  #local Start = allBones[boneNumber][Starting];
  #debug concat(" starting: <",str(Start.x,4,2),",",
    str(Start.y,4,2),",",str(Start.z,4,2),">") 
  #local End = allBones[boneNumber][Ending];    
  #debug concat(" ending: <",str(End.x,4,2),",",str(End.y,4,2),",",str(End.z,4,2),
    ">\n    ")    
  #local X = allBones[boneNumber][AxisX];
  #debug concat(" X axis: <",str(X.x,4,3),",",str(X.y,4,3),",",str(X.z,4,3),"> ")
  #local Y = allBones[boneNumber][AxisY];
  #debug concat(" Y axis: <",str(Y.x,4,3),",",str(Y.y,4,3),",",str(X.z,4,3),"> ")
  #local Z = allBones[boneNumber][AxisZ];
  #debug concat(" Z axis: <",str(Z.x,4,3),",",str(Z.y,4,3),",",str(Z.z,4,3),"> ")
  #local Rot = allBones[boneNumber][AbsRot];
  #debug concat(" rotation: <",str(Rot.x,4,3),",",
    str(Rot.y,4,3),",",str(Rot.z,4,3),"> \n    ")
  #local EXT = findBoneExtension(boneNumber);
  #debug concat(" bone extension: <",str(EXT.x,8,6),",",str(EXT.y,8,6),",",
    str(EXT.z,8,6),"> ")
  //#debug concat(" object: ",BoneObject[boneNumber]," ")
  // add or comment out more as you like  
  #debug"\n"
#end   



#macro debugSkeleton (SkelName) 
// displays bone data for all bones of one skeleton in the Messages window
  #debug concat("\n skeleton: ",SkelName,"\n")
  #local numBones = getNumBones();
  markSkeleton(SkelName)
  #local Index = 0;
  #while (Index < numBones)
    #if (BoneUsed[Index] > 0)
      debugBone(Index)
    #end
    #declare Index = Index + 1;
  #end
  #debug "\n"
#end



#macro debugAllBones () 
// displays bone data for all bones in the Messages window
  #local numBones = getNumBones();
  #local Index = 0;
  #while (Index<numBones)
    debugBone(Index)
    #declare Index = Index + 1;
  #end 
  #debug "\n"
#end



#macro debugAllSkels() 
// displays skeleton array data for all skeletons in the Messages window
  #local numSkels = getNumSkels();
  #local Index = 0;
  #while (Index<numSkels)
    #debug concat("\n",str(Index,2,0)," ",str(SkeletonPtr[Index],2,0)," ",
      SkeletonNames[Index],"\n")
    #declare Index = Index + 1;
  #end 
  #debug "\n"
#end 



#macro debugSkelsAndBones()
// displays array data for all skeletons and bones in the Messages window
  debugAllSkels()
  debugAllBones()
#end




///////////////////////////////////////////////////////////////////////////////////
//  end of bones.inc
///////////////////////////////////////////////////////////////////////////////////