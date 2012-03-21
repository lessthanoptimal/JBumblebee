package boofcv.drivers.stereo;

import boofcv.core.image.ConvertBufferedImage;
import boofcv.struct.image.ImageUInt8;
import boofcv.struct.image.MultiSpectral;

import java.awt.image.BufferedImage;

/**
 * @author Peter Abeles
 */
public class CameraBumblebee2 {

	MultiSpectral<ImageUInt8> ms;
	BufferedImage image;
	byte rgb[];
	int width;
	int height;

	static {
		System.loadLibrary("jbumblebee");
	}
	
	public boolean init( Interpolation interpolation , int framesPerSecond , int maxLagMilli ) {
		if( _init(interpolation.ordinal(),framesPerSecond,maxLagMilli ) ) {
			this.width = _getWidth();
			this.height = _getHeight();
			rgb = new byte[3*width*height];
			return true;
		}
		return false;
	}

	public native void shutdown();

	public boolean grabframe() {
		return grabRGB(rgb);
	}
	
	public BufferedImage getBufferedImage() {
		int w = width;
		int h = height;
		
		if( image == null || image.getWidth() != w || image.getHeight() != h ) {
			image = new BufferedImage(w,h,BufferedImage.TYPE_INT_RGB);
		}

		MultiSpectral<ImageUInt8> imageMS = getMultiSpectral();

		ImageUInt8 r = imageMS.getBand(0);
		ImageUInt8 b = imageMS.getBand(2);

		imageMS.bands[0] = b;
		imageMS.bands[2] = r;

		ConvertBufferedImage.convertTo_U8(imageMS,image);

		// Buffered images are so slow that this is faster....
//		int index = 0;
//		for( int y = 0; y < h; y++ ) {
//			for( int x = 0; x < w; x++ ) {
//				int c = ((rgb[index++]&0xff) << 16) | ((rgb[index++]&0xff) << 8) | ((rgb[index++]&0xff));
//				image.setRGB(x,y,c);
//			}
//		}

		return image;
	}
	
	public MultiSpectral<ImageUInt8> getMultiSpectral() {
		int w = width;
		int h = height;

		if( ms == null || ms.getWidth() != w || ms.getHeight() != h ) {
			ms = new MultiSpectral<ImageUInt8>(ImageUInt8.class,w,h,3);
		}
		
		ImageUInt8 r = ms.getBand(0);
		ImageUInt8 g = ms.getBand(1);
		ImageUInt8 b = ms.getBand(2);

		int index = 0;
		int N = w*h;
		for( int i = 0; i < N; i++ ) {
			b.data[i] = rgb[index++];
			g.data[i] = rgb[index++];
			r.data[i] = rgb[index++];
		}

		return ms;
	}
	
	public byte[] getRGB() {
		return rgb;
	}
	
	public int getWidth() {
		return width;
	}
	
	public int getHeight() {
		return height;
	}

	private native boolean grabRGB( byte data[] );

	private native boolean _init( int interpolation , int fps , int maxLagMilli );

	public native int _getWidth();

	public native int _getHeight();

	public native long getTimeStamp();
	
	public static enum Interpolation {
		NEAREST,
		SIMPLE,
		BILINEAR,
		HQLINEAR,
		DOWNSAMPLE,
		EDGESENSE,
		VNG,
		AHD
	}
}
