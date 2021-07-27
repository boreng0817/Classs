package assignment;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Calendar;

public class TravelInfoRequest {
	public static void main (String[] args) {
		BufferedReader br = null;
		BufferedWriter bw = null;
		String str = null, letter = "";
		KeyValue[] kv = null;
		Calendar calendar = null;
		Distance[] dist = null;
		Countries coun = null;
		int i = 0;
		
		try {
		br = new BufferedReader( new FileReader("template_file.txt"));
		while((str = br.readLine()) != null) {
			letter = letter + str + "\r\n";
		}
		
		br.close();
		
		calendar = Calendar.getInstance();
		kv = new KeyValue[7];
		kv[0] = new KeyValue("date", 
				""  + calendar.get(Calendar.YEAR) + 
				"-" + (calendar.get(Calendar.MONTH) + 1) +
				"-" + calendar.get(Calendar.DATE) );		
		br = new BufferedReader( new FileReader("properties.txt") );
		dist = new Distance[2];
		for( i = 1 ; i < 7 ; ++i ) {
			kv[i] = new KeyValue(br.readLine());
			if((kv[i].getKey()).equals("startcountry")) {
				coun = new Countries(kv[i].getValue());
				dist[0] = new Distance(coun.getCountry(), coun.getLat(), coun.getLng());
			}
			if((kv[i].getKey()).equals("departcountry")) {
				coun = new Countries(kv[i].getValue());
				dist[1] = new Distance(coun.getCountry(), coun.getLat(), coun.getLng());
			}
		}
		
		br.close();
		
		for( i = 0 ; i < 7 ; ++i ) {
			letter = letter.replace("{" + kv[i].getKey() + "}", kv[i].getValue() );
		}
		
		letter = letter.replaceAll("<add info>", Distance.getDistance(dist[0], dist[1]) );
		
		bw = new BufferedWriter( new FileWriter("output.txt") );
		bw.write(letter);
		
		bw.close();
		} catch(FileNotFoundException e) {
			System.out.println("no file in main");
			e.getStackTrace();
		} catch (IOException e) {
			System.out.println("ioexcep in main");
			e.getStackTrace();
		}
	}
}
