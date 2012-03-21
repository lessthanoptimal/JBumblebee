package boofcv.drivers.stereo;

import boofcv.gui.image.ShowImages;

import java.awt.image.BufferedImage;

/**
 * Grabs images from the camera as quick as possible and computes the FPS
 *
 * @author Peter Abeles
 */
public class SpeedTestApp {
	public static void main( String args[] ) {
		CameraBumblebee2 camera = new CameraBumblebee2();
		
		if( !camera.init(CameraBumblebee2.Interpolation.HQLINEAR,0,200) )
			throw new RuntimeException("Init failed!");

		int N = 100;
		long before = System.currentTimeMillis();
		
		for( int i = 0; i < N; i++ ) {
			if( !camera.grabframe() ) {
				throw new RuntimeException("Grab frame failed");
			}
			System.out.println("i = "+i+"  ts "+camera.getTimeStamp());
		}
			
		long elapsed = System.currentTimeMillis()-before;
		
		System.out.println("Time "+elapsed);
		System.out.println("FPS  "+(N/(elapsed/1000.0)));

		System.out.println("dimension = " + camera.getWidth() + "  " + camera.getHeight());

		camera.shutdown();
		
		System.out.println("Done");
	}
}
