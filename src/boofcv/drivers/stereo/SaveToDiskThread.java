package boofcv.drivers.stereo;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 * Saving to disk seems to experience very long blocking delays causing images to be dropped.  To get around this
 * problem output is queued up and saved to desk.
 *
 * @author Peter Abeles
 */
public class SaveToDiskThread extends Thread {

	int w,h;

	OutputStreamWriter timeLog;
	List<String> queueLog = new Vector<String>();
	List<byte[]> queueImage = new ArrayList<byte[]>();
	volatile int startImage;
	volatile int sizeImage;

	int imageCount;

	public SaveToDiskThread(int w, int h, int queueSize, OutputStreamWriter timeLog) {
		this.w = w;
		this.h = h;
		this.timeLog = timeLog;

		int length = w*h*3;

		for( int i = 0; i < queueSize; i++ ) {
			queueImage.add(new byte[length]);
		}
	}

	public synchronized void push( byte[] data ) {
		if( sizeImage >= queueImage.size() )
			throw new RuntimeException("Overloaded queue");

		int index = (startImage + sizeImage)% queueImage.size();

		byte[] a = queueImage.get(index);
		System.arraycopy(data,0,a,0,a.length);

		sizeImage++;
	}

	public synchronized void push( String text ) {
		queueLog.add(text);
	}

	public void blockUntilEmpty() {
		while( sizeImage > 0 && queueLog.size() > 0 ) {
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
			}
		}
	}

	public void run() {
		while( true ) {
			doSaveFileLoop();

			try {
				Thread.sleep(10);
				System.gc();
			} catch (InterruptedException e) {
			}
		}
	}

	private void doSaveFileLoop() {
		try {
			while( sizeImage > 0 ) {

				CaptureSequence.savePPM(queueImage.get(startImage), w, h, imageCount);

				synchronized ( this ) {
					startImage = (startImage + 1) % queueImage.size();
					sizeImage--;
					imageCount++;
				}
			}

			while( queueLog.size() > 0 ) {
				String line = queueLog.remove(0);
				timeLog.write(line);
				timeLog.flush();
			}
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
	}
}
