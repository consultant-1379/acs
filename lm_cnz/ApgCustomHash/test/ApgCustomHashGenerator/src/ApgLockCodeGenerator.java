
import com.ericsson.apg.keygen.*;

public class ApgLockCodeGenerator 
{

	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		if(args.length == 1)
		{
			String myFingerPrint = args[0];
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
		}
		else
		{
		   System.out.println("Invalid usage");  		
		   System.out.println("usage: java -jar ApgCustomHashCodeGenerator.jar <fingerprint>");  		
		}
		
	}

}
