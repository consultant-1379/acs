import os
import getopt
import sys
import time
from glob import glob


dirName = "/home1/SSU/ACS_SSU_DIR_TEST/"
lastcreatedFileIndex=0

def vStartInMode():
   contin=True
   fileHandles = []

   while contin:
     print "\n Enter \n 1.Create File Handles \n2. Delete FileHandles \n3.Exit"

     choice = raw_input()
     if choice=="1":
        noofHandles = int(raw_input())
        createFileHandles(noofHandles,fileHandles)
     elif choice=="2":
        noOfHandlesToDelete = int(raw_input())
        deleteFileHandles(noOfHandlesToDelete,fileHandles)
     else:
        contin = False
 
def createFileHandles(noOfHandles,fileHandles):
   print "Creating Files %s" % (noOfHandles)
   testdir = os.path.dirname(dirName)

   if not os.path.exists( testdir ):
      os.makedirs(testdir)

   global lastcreatedFileIndex
   newStartIndex =  lastcreatedFileIndex+1
   for x in range(newStartIndex,newStartIndex+noOfHandles):
     fileName = dirName+str(x)+".txt"
     fileHandles.append(open(fileName,'w'))
     lastcreatedFileIndex = x
   time.sleep(sleepForSec)

def deleteFileHandles(noFilesToDelete,fileHandles):

   global lastcreatedFileIndex
   if lastcreatedFileIndex == 0:
      return 

   startIndex = 0

   if noFilesToDelete <=lastcreatedFileIndex:
     startIndex = lastcreatedFileIndex-noFilesToDelete

   print "Last Createf File Index:%d Del Start Index:%d" % ( lastcreatedFileIndex,startIndex )

   sIndex = startIndex
   eIndex = lastcreatedFileIndex
   
   while sIndex>eIndex:
      try:
         fileHandles.remove(sIndex)
         sIndex-=1
      except Exception,e:
         print "%s" , str(e)
         sIndex = 0
         lastcreatedFileIndex = sIndex-1
   lastcreatedFileIndex = sIndex-1
   deleteFiles(startIndex)


def deleteFiles(startIndex):
   if startIndex == 1 :
      for f in glob(dirName+"*.txt"):
         os.unlink(f)
   else:
      try:
        while True:
           fileName = dirName+str(startIndex)+".txt"  
           os.unlink(fileName)
           startIndex +=1 
      except Exception,e:
         print "Finished Deleting file till %d.txt" % startIndex


                  


if __name__=='__main__':
   
   noOfHandles = 200000
   sleepForSec = 0
   create=True
   fileHandles = []
   interactive = False
 
   deleteFiles(1) 
   
   options,remainder = getopt.getopt(sys.argv[1:],"cdn:s:i",["no=","sleep=","delete","interactive"])   
   
   for opt,arg in options:
     if opt in ("-n","--no"):
        noOfHandles = int(arg)
     elif opt in ("-s","--sleep"):
       sleepForSec = int(arg)
     elif opt in ("-d","--delete"):
       create=False
     elif opt in ("-i","--interactive"):
       interactive=True
     else:
	print "Usage: FileHandle.py ( -c(create)|-d(delete)) -n [No of File Handles to create/ Delete Start Index] "

   if interactive:
      print "Starting Interactive Mode"
      vStartInMode()
   elif create:
      createFileHandles(noOfHandles,fileHandles)
   
   deleteFiles(1)

 

