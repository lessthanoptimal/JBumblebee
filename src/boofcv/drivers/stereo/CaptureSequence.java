package boofcv.drivers.stereo;

import boofcv.gui.image.ImagePanel;
import boofcv.gui.image.ShowImages;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

/**
 * @author Peter Abeles
 */
public class CaptureSequence {

	public static void main( String args[] ) throws IOException {
		CameraBumblebee2 camera = new CameraBumblebee2();

		if( !camera.init(CameraBumblebee2.Interpolation.HQLINEAR,10,200) )
			throw new RuntimeException("Init failed!");

		System.out.println("Image Dimension = "+camera.getWidth()+"  "+camera.getHeight());

		// create a display
		if( !camera.grabframe() ) {
			throw new RuntimeException("Grab frame failed");
		}
		ImagePanel gui = new ImagePanel(camera.getBufferedImage());
		ShowImages.showWindow(gui,"Stereo Images");

		// storage for left and right images
		int w = camera.getWidth();
		int h = camera.getHeight();
		BufferedImage left = new BufferedImage(w,h/2,BufferedImage.TYPE_INT_RGB);
		BufferedImage right = new BufferedImage(w,h/2,BufferedImage.TYPE_INT_RGB);

		for( int i = 0; i < 1000; i++ ) {

			if( !camera.grabframe() ) {
				throw new RuntimeException("Grab frame failed");
			}
			System.out.println("Processing image "+i+"    ts "+camera.getTimeStamp()/1000+" "+System.currentTimeMillis());

			BufferedImage image = camera.getBufferedImage();
			gui.setBufferedImage(image);
			gui.repaint();

			left.createGraphics().drawImage(image,0,0,w,h/2,0,0,w,h/2,null);
			right.createGraphics().drawImage(image,0,0,w,h/2,0,h/2,w,h,null);

			ImageIO.write(left,"png",new File(String.format("images/left%05d.png",i)));
			ImageIO.write(right,"png",new File(String.format("images/right%05d.png",i)));
		}

		camera.shutdown();

		System.out.println("Done");
	}
}
