package boofcv.drivers.stereo;

import boofcv.core.image.ConvertBufferedImage;
import boofcv.gui.image.ShowImages;
import boofcv.struct.image.ImageUInt8;
import boofcv.struct.image.MultiSpectral;

import java.awt.image.BufferedImage;

/**
 * Grabs an image from the camera and displays it
 *
 * @author Peter Abeles
 */
public class GrabDisplayStereo {
	public static void main( String args[] ) {
		CameraBumblebee2 camera = new CameraBumblebee2();
		
		if( !camera.init(CameraBumblebee2.Interpolation.HQLINEAR,0,200) )
			throw new RuntimeException("Init failed!");

		if( !camera.grabframe() )
			throw new RuntimeException("Grab frame failed");

		System.out.println("dimension = "+camera.getWidth()+"  "+camera.getHeight());

		BufferedImage image = camera.getBufferedImage();

		ShowImages.showWindow(image,"Stereo Image");

		camera.shutdown();
		
		System.out.println("Done");
	}
}
