package boofcv.drivers.stereo;

import boofcv.gui.image.ImagePanel;
import boofcv.gui.image.ShowImages;

import javax.imageio.IIOImage;
import javax.imageio.ImageIO;
import javax.imageio.ImageWriteParam;
import javax.imageio.ImageWriter;
import javax.imageio.stream.FileImageOutputStream;
import java.awt.image.BufferedImage;
import java.io.*;

/**
 * @author Peter Abeles
 */
public class CaptureSequence {
	
	public static boolean shutdownRequest = false;

	private static void addShutdown() {
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				shutdownRequest = true;
				try { Thread.sleep(200); } catch (InterruptedException e) { e.printStackTrace(); }
			}
		});
	}

	private static void savePPM(CameraBumblebee2 camera, int w, int h, int i) throws IOException {
		File out = new File(String.format("images/left%07d.ppm",i));
		BufferedOutputStream os = new BufferedOutputStream(new FileOutputStream(out));
		String header = String.format("P6\n%d %d\n255\n", w, h/2);
		os.write(header.getBytes());
		os.write(camera.getRGB(), 0, w * h * 3 / 2);
		os.close();
		out = new File(String.format("images/right%07d.ppm",i));
		os = new BufferedOutputStream(new FileOutputStream(out));
		os.write(header.getBytes());
		os.write(camera.getRGB(),w*h*3/2,w*h*3/2);
		os.close();
	}

	public static void main( String args[] ) throws IOException {
		int fps = 20;
		int totalCapture = 100;
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
		System.out.println("fps = "+fps+" total "+totalCapture+" show images = "+showImages);

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
//		BufferedImage left = new BufferedImage(w,h/2,BufferedImage.TYPE_INT_RGB);
//		BufferedImage right = new BufferedImage(w,h/2,BufferedImage.TYPE_INT_RGB);

		OutputStreamWriter timeLog = new OutputStreamWriter(new FileOutputStream("images/time.txt")); 
		timeLog.write("# (frame #) (time stamp ms)\n");

//		ImageWriter writer = ImageIO.getImageWritersByFormatName("jpeg").next();
//		ImageWriteParam iwp = writer.getDefaultWriteParam();
//		iwp.setCompressionMode(ImageWriteParam.MODE_EXPLICIT);
//		iwp.setCompressionQuality(1);

		addShutdown();

		long before = System.currentTimeMillis();

		int i = 0;
		for( ; !shutdownRequest && (totalCapture == 0 || i < totalCapture); i++ ) {

			if( !camera.grabframe() ) {
				throw new RuntimeException("Grab frame failed");
			}
			System.out.println("Processing image "+i+"    ts "+camera.getTimeStamp()/1000+" "+System.currentTimeMillis());

			if( gui != null ) {
				BufferedImage image = camera.getBufferedImage();
				gui.setBufferedImage(image);
				gui.repaint();
			}

//			left.createGraphics().drawImage(image,0,0,w,h/2,0,0,w,h/2,null);
//			right.createGraphics().drawImage(image,0,0,w,h/2,0,h/2,w,h,null);
//
//			FileImageOutputStream output;
//			output = new FileImageOutputStream(new File((String.format("images/left%07d.jpg",i))));
//			writer.setOutput(output);
//			writer.write(null,new IIOImage(left, null, null),iwp);
//			output.close();

//			output = new FileImageOutputStream(new File((String.format("images/right%07d.jpg",i))));
//			writer.setOutput(output);
//			writer.write(null,new IIOImage(right, null, null),iwp);
//			output.close();

			timeLog.write(String.format("%07d %d\n",i,(camera.getTimeStamp()/1000)));
			timeLog.flush();

			savePPM(camera, w, h, i);
		}

		System.out.println("actual FPS = "+(i*1000.0/(System.currentTimeMillis()-before)));
		
		timeLog.close();
		camera.shutdown();

		System.out.println("Done");
		System.exit(0);
	}
}
