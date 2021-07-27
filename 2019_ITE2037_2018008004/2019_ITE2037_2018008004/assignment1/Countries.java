package assignment;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.StringTokenizer;

public class Countries {
	private String country;
	private double lat;
	private double lng;
	
	public String getCountry() {
		return country;
	}

	public double getLat() {
		return lat;
	}

	public double getLng() {
		return lng;
	}

	public Countries(String str)  {
		String temp = "";
		BufferedReader br = null;
		try {
			br = new BufferedReader(new FileReader("Countries.csv"));
			while( !temp.equals(str) ) {
				StringTokenizer st = new StringTokenizer(br.readLine(), ",");
				temp = st.nextToken();
				lat = Double.parseDouble(st.nextToken());
				lng = Double.parseDouble(st.nextToken());
			}
			country = str;
		} catch (FileNotFoundException e ) {
			System.out.println("filenotfound in countries");
			e.printStackTrace();
		} catch (IOException e ) {
			System.out.println("ioexcep in countries");
			e.printStackTrace();
		} finally {
			if ( br != null ) {
				try {
					br.close();
				} catch ( IOException e ) {
					System.out.println("ioexcep in countries(2)");
					e.printStackTrace();
				}
			}
		}
	}
}
