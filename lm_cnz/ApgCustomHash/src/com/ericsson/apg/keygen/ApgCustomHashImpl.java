package com.ericsson.apg.keygen;

//import java.io.*;
//import java.net.URL;
import java.util.Properties;
import java.util.regex.Pattern;
//import java.util.Properties;
import com.safenet.poetic.keygen.*;

import com.sfnt.rms.*;
import com.sfnt.rms.structs.*;
//import com.sfnt.rms.exception.*;

public final class ApgCustomHashImpl implements ExtendedCustomHash 
{
	//private Handle32 handle = new Handle32();
	private int status = 0;
    String property_file_path=new String("rmscaffe.properties"); //Location of file is same directory as JAR file
	private int lock_selector_in=1024; 
    MachineId machineId_new=new MachineId(true);
    Int32Buffer lock_selector_out=new Int32Buffer();
    int lockCodeLen=100;
    NativeStringBuffer lockCode=new NativeStringBuffer(100);
    String theFormatLockingCode = new String("");
    int theOS 	 = 0;
    int theOSBit = 0;
    String theCurrentDirectory = new String("");
    
    private boolean extractFilesLinux() throws Exception
    {
        	    	
    	java.io.File myLinuxDir = new java.io.File(theCurrentDirectory + java.io.File.separator + "Sentinel_Linux");
    	    	
    	myLinuxDir.mkdir();
    	
    	String myLibNamelssrv = new String("");
    	String myLibOUTNamelssrv = new String("");
    	String myLibNamelsinit = new String("");
    	String myLibOUTNamelsinit = new String("");
    	String myLibNamelsdcod = new String("");
    	String myLibOUTNamelsdcod = new String("");
    	
    	if(theOSBit == 32)  
    	{
    		myLibNamelssrv 		= "Linux/liblssrv.so";
    		myLibOUTNamelssrv   = "liblssrv.so";
    		myLibNamelsinit     = "Linux/liblsinit.so";
    		myLibOUTNamelsinit  = "liblsinit.so";
    		myLibNamelsdcod     = "Linux/libdecod.so";
    		myLibOUTNamelsdcod  = "libdecod.so";
    	}
    	else if(theOSBit == 64)
    	{
    		myLibNamelssrv 		= "Linux/liblssrv64.so";
    		myLibOUTNamelssrv   = "liblssrv64.so";
    		myLibNamelsinit     = "Linux/liblsinit64.so";
    		myLibOUTNamelsinit  = "liblsinit64.so";
    		myLibNamelsdcod     = "Linux/libdecod64.so";
    		myLibOUTNamelsdcod  = "libdecod64.so";
    	}
    	else
    	{
    		throw new Exception("Incompatible Operating system Bit architecture");
    	}
    			
    	try
		{
			createLibraryFile(myLinuxDir,myLibNamelssrv,myLibOUTNamelssrv);
			createLibraryFile(myLinuxDir,myLibNamelsinit,myLibOUTNamelsinit);
			createLibraryFile(myLinuxDir,myLibNamelsdcod,myLibOUTNamelsdcod);
		}
		catch(Exception e)
		{
			throw e;
		}
			
		return true;
    }
   
    
    private boolean extractFilesWindows() throws Exception
    {
    	   	
    	java.io.File myWindowsDir = new java.io.File(theCurrentDirectory + java.io.File.separator + "Sentinel_Windows");
    	    	
    	myWindowsDir.mkdir();
    	
    	String myLibNamelsapi 		= new String("");
    	String myLibOUTNamelsapi 	= new String("");
    	String myLibNamelsdcod 		= new String("");
    	String myLibOUTNamelsdcod 	= new String("");
    	String myLibNamelsinit 		= new String("");
    	String myLibOUTNamelsinit 	= new String("");
    	
    	if(theOSBit == 32)  
    	{
    		myLibNamelsapi 		= "Windows/lsapiw32.dll";
    		myLibOUTNamelsapi   = "lsapiw32.dll";
    		myLibNamelsdcod     = "Windows/lsdcod32.dll";
    		myLibOUTNamelsdcod  = "lsdcod32.dll";
    		myLibNamelsinit     = "Windows/lsinit32.dll";
    		myLibOUTNamelsinit  = "lsinit32.dll";
    	}
    	else if(theOSBit == 64)
    	{
    		myLibNamelsapi 		= "Windows/lsapiw64.dll";
    		myLibOUTNamelsapi   = "lsapiw64.dll";
    		myLibNamelsdcod     = "Windows/lsdcod64.dll";
    		myLibOUTNamelsdcod  = "lsdcod64.dll";
    		myLibNamelsinit     = "Windows/lsinit64.dll";
    		myLibOUTNamelsinit  = "lsinit64.dll";
    	}
    			
    	try
		{
			createLibraryFile(myWindowsDir,myLibNamelsapi,myLibOUTNamelsapi);
			createLibraryFile(myWindowsDir,myLibNamelsdcod,myLibOUTNamelsdcod);
			createLibraryFile(myWindowsDir,myLibNamelsinit,myLibOUTNamelsinit);
		}
		catch(Exception e)
		{
			throw e;
		}
			
		return true;
    }
    private void createLibraryFile(java.io.File aOutDir,String InFilePath, String OutFilePath) throws Exception
    {
    	java.io.File myFile = new java.io.File(aOutDir + java.io.File.separator +OutFilePath);
		if(!myFile.exists())
		{
			java.io.InputStream myInputStream = getClass().getClassLoader().getResourceAsStream(InFilePath);
			java.io.FileOutputStream myOutputStream = new java.io.FileOutputStream(myFile);   
			while (myInputStream.available() > 0) 
			{  // write contents of 'is' to 'fos'    
				myOutputStream.write(myInputStream.read());    
			}  
			myOutputStream.close();  
			myInputStream.close();
		}
    	
    }
    
    private int getOperatingSystem()
    {
	  	int theOS = 0;
	 	String os = System.getProperty("os.name").toLowerCase();
		if(os.indexOf("win") >= 0)
		{
		  theOS = 1;
		}
		else if((os.indexOf("nix") >= 0 || os.indexOf("nux") >= 0))
		{
			theOS = 2;
		}
		else
		{
			theOS = 0;
		}
		return theOS;
    }
    private int getOSBit()
    {
    	int theOSBit = 0;
    	String myBitStr = System.getProperty("os.arch");
    	if(myBitStr.contains("64"))
    	{
    		theOSBit = 64;    		
    	}
    	else if(myBitStr.contains("86"))
    	{
    		theOSBit = 32;
    	}
    	return theOSBit;
    }
	@Override
	public String getExtendedCustomHashedLockingId(String lockingIdToHash)
			throws Exception 
	{
		
		try
		{
			if( lockingIdToHash.length() > 64)
			{
				throw new Exception("Invalid Input - String length is more than 64 characters");
			}
			if ( isInputStringValid(lockingIdToHash) == false)
			{
				throw new Exception("Invalid Input - Valid Input is NodeName-IPAddress");	
			}	
						
			theOS = getOperatingSystem();
						
			if(theOS == 0)
	    	{
				throw new Exception("The Operating System is neither Windows nor Linux " );
	    	}
			
			theOSBit = getOSBit();
			
	    	if(theOSBit == 0)
	    	{
	    		throw new Exception("Incompatible Operating system Bit architecture");
	    	}
	    	
	    	theCurrentDirectory = System.getProperty("user.dir");
	    	
	    	LSAPIHelper lsapihelper=new LSAPIHelper();
	    	
	    	Properties theProperties = new Properties();
	    	
			if(theOS == 2) //Linux
			{
				try
				{
					extractFilesLinux();
				}
				catch(Exception e1)
				{
					throw new Exception("Problem in extracting Linux Sentinel Libraries -" + e1.getMessage());
				}
					
		    	if(theOSBit == 32)
		    	{
		    		theProperties.setProperty("rms.client.library.name", "./Sentinel_Linux/liblssrv.so");
					theProperties.setProperty("rms.initialization.library.name", "./Sentinel_Linux/liblsinit.so");
					theProperties.setProperty("rms.decode.library.name", "./Sentinel_Linux/libdecod.so");
		    		
		    	}
		    	else if(theOSBit == 64)
		    	{
		    		theProperties.setProperty("rms.client.library.name", "./Sentinel_Linux/liblssrv64.so");
					theProperties.setProperty("rms.initialization.library.name", "./Sentinel_Linux/liblsinit64.so");
					theProperties.setProperty("rms.decode.library.name", "./Sentinel_Linux/libdecod64.so");
		    	}
		    	else
		    	{
		    		throw new Exception("Incompatible Operating system Bit architecture"); 
		    	}
			}
			else if(theOS == 1)//Windows
			{
				try
				{
					extractFilesWindows();				
				
				}
				catch(Exception e2)
				{
					throw new Exception("Problem in extracting Windows Sentinel Libraries -" + e2.getMessage());
				}
		    	
		    	if(theOSBit == 32)
		    	{
					String myStr = theCurrentDirectory + "\\Sentinel_Windows\\lsapiw32.dll";
					theProperties.setProperty("rms.client.library.name", myStr);
					myStr = theCurrentDirectory + "\\Sentinel_Windows\\lsinit32.dll";
					theProperties.setProperty("rms.initialization.library.name", myStr);
					myStr = theCurrentDirectory + "\\Sentinel_Windows\\lsdcod32.dll";
					theProperties.setProperty("rms.decode.library.name", myStr);
		    	}
				else if(theOSBit == 64)
				{
					String myStr = theCurrentDirectory + "\\Sentinel_Windows\\lsapiw64.dll";
					theProperties.setProperty("rms.client.library.name", myStr);
					myStr = theCurrentDirectory + "\\Sentinel_Windows\\lsinit64.dll";
					theProperties.setProperty("rms.initialization.library.name", myStr);
					myStr = theCurrentDirectory + "\\Sentinel_Windows\\lsdcod64.dll";
					theProperties.setProperty("rms.decode.library.name", myStr);
					
				}
				else
				{
					throw new Exception("Incompatible Operating system Bit architecture");
				}
		    	
			}
			else
			{
				 throw new Exception("The Operating System is neither Windows nor Linux " );
			}
			
			lsapihelper.init(theProperties);
			
			/** Call to Initialize the RMS client library*/
			
			status = lsapihelper.VLS_INITIALIZE();
			
			/** Call to initialize MachineID structure*/

			ApgCustomEx myAxeCustomEx = new ApgCustomEx();
			
			myAxeCustomEx.setInputString(lockingIdToHash);
			
			lsapihelper.VLSsetCustomExFunc(myAxeCustomEx);
			
			status = lsapihelper.VLSinitMachineID(machineId_new);
			
  			status = lsapihelper.VLSgetMachineID(lock_selector_in,machineId_new,lock_selector_out);
  			 
  			status = lsapihelper.VLSmachineIDToLockCodeEx(machineId_new,lock_selector_out.get(),lockCode,lockCodeLen,0);
  			 
  			if(status==LSAPI.LS_SUCCESS)
			{
  				formatLockingCode(Integer.toHexString(lock_selector_out.get()),lockCode.buffer.get());
			}
			/** Call to release the client library resources*/
			lsapihelper.VLS_CLEANUP();	
		
		}
		catch(Exception e)
		{
			throw e;
		}
		return theFormatLockingCode;
	}
	private int formatLockingCode(String alockSelectorOut,String alockCode)
	{
		theFormatLockingCode = alockSelectorOut;
		theFormatLockingCode += '-';
		if (alockCode.length() == 0)
		{
			theFormatLockingCode = "";
			return 0;
		}
		int myCounter = 0;
		for(;myCounter < alockCode.length();myCounter++ )
		{
			theFormatLockingCode += alockCode.charAt(myCounter);
			if((myCounter+1) % 4 == 0)
			{	if( (myCounter+1) != 16)
				{theFormatLockingCode += ' ';}
			}
		}
		//System.out.println("The LockingCode is :" + theFormatLockingCode);
		return 1;
		
	}
	public boolean isInputStringValid(String aInputString) 
	{
		boolean validFlag = false;
		
		Pattern  myPattern = Pattern.compile("^([a-zA-Z0-9]+)\\-([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([01]?\\d\\d?|2[0-4]\\d|25[0-5])$");
		validFlag =  myPattern.matcher(aInputString).matches();
		return validFlag;
	}
@Override
	public long getCustomHashedLockingId(String lockingIdToHash) throws Exception 
	{
	// TODO Auto-generated method stub
		return 0;
	}
	/*public static void main(String[] args) 
	{
	
		String myFingerPrint = "1234567890-3.3.3.3";
		String LockCodeStr = new String("");	
		try
		{
			 ApgCustomHashImpl myAxeCustomHashImpl = new ApgCustomHashImpl();
			 LockCodeStr = myAxeCustomHashImpl.getExtendedCustomHashedLockingId(myFingerPrint);	
			 System.out.println("The LockingCode is :" + LockCodeStr);
		}
		catch(Exception e)
	     {
		   System.out.println("API call fail with message:"+e.getMessage());  		
		  
		 }			
	
	}*/

}/**End of ApgCustomHashImpl class*/

