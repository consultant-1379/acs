package com.ericsson.apg.keygen;



/*import java.io.*;
import java.nio.ByteBuffer;

import com.sfnt.rms.*;*/
//import java.util.regex.Pattern;

import com.sfnt.rms.LSAPIHelper.ICallbackEx;
import com.sfnt.rms.structs.*;
/*import com.sfnt.rms.exception.*;
import javolution.io.*;
import javolution.io.Struct.UTF8String;
import javolution.io.Struct.Signed32;*/

public class ApgCustomEx implements ICallbackEx 
{
	String theInputString = new String("");
	

	public void setInputString(String aInputString)
	{
		
		theInputString = aInputString;
		
	}
	
	@Override
	public int callback(CustomEx arg0, Int32Buffer arg1) 
	{
		//System.out.println("start callback()");
		// TODO Auto-generated method stub
		arg0.customEx.set(theInputString);
		arg0.len.set(theInputString.length());
		arg1.set(1);
		//System.out.println("end callback()");
		return 0;
	}

}
