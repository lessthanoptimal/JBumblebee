package boofcv.drivers.stereo;

import boofcv.gui.image.ImagePanel;
import boofcv.gui.image.ShowImages;
import boofcv.struct.image.ImageUInt8;
import boofcv.struct.image.MultiSpectral;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;

/**
 * @author Peter Abeles
 */
public class CaptureSequence {

	public static void main( String args[] ) throws IOException {
		int fps = 10;
		int totalCapture = 10;
		boolean showImages = true;

		if( args.length >= 1 ) {
			if( args[0].charAt(0) == '-') {
				System.out.println("(total frames) (fps) (show images [0,1])");
				return;
			}
			totalCapture = Integer.parseInt(args[0]);
			fps = Integer.parseInt(args[1]);
			showImages = Integer.parseInt(args[2]) != 0;
		}

		CameraBumblebee2 camera = new CameraBumblebee2();

		if( !camera.init(CameraBumblebee2.Interpolation.HQLINEAR,fps,200) )
			throw new RuntimeException("Init failed!");

		System.out.println("Image Dimension = "+camera.getWidth()+"  "+camera.getHeight());

		// create a display
		if( !camera.grabframe() ) {
			throw new RuntimeException("Grab frame failed");
		}
		ImagePanel gui = showImages ? new ImagePanel(camera.getBufferedImage()) : null;
		if( gui != null )
			ShowImages.showWindow(gui,"Stereo Images");

		// storage for left and right images
		int w = camera.getWidth();
		int h = camera.getHeight();
		BufferedImage left = new BufferedImage(w,h/2,BufferedImage.TYPE_INT_RGB);
		BufferedImage right = new BufferedImage(w,h/2,BufferedImage.TYPE_INT_RGB);

		long before = System.currentTimeMillis();
		
		for( int i = 0; totalCapture == 0 || i < totalCapture; i++ ) {

			if( !camera.grabframe() ) {
				throw new RuntimeException("Grab frame failed");
			}
			System.out.println("Processing image "+i+"    ts "+camera.getTimeStamp()/1000+" "+System.currentTimeMillis());

			BufferedImage image = camera.getBufferedImage();
			if( gui != null ) {
				gui.setBufferedImage(image);
				gui.repaint();
			}

			left.createGraphics().drawImage(image,0,0,w,h/2,0,0,w,h/2,null);
			right.createGraphics().drawImage(image,0,0,w,h/2,0,h/2,w,h,null);
//
			ImageIO.write(left,"jpg",new File(String.format("images/left%06d.jpg",i)));
			ImageIO.write(right,"jpg",new File(String.format("images/right%06d.jpg",i)));

//			ImageIO.write(image,"jpg",new File(String.format("images/image%06d.jpg",i)));

//			File out = new File(String.format("images/image%06d.ppm",i));
//			BufferedOutputStream os = new BufferedOutputStream(new FileOutputStream(out));
//			String header = String.format("P6\n%d %d\n255\n", w, h);
//			os.write(header.getBytes());
//			os.write(camera.getRGB());
//			os.close();
		}

		System.out.println("actual FPS = "+(totalCapture*1000.0/(System.currentTimeMillis()-before)));
		
		camera.shutdown();

		System.out.println("Done");
		System.exit(0);
	}
}
