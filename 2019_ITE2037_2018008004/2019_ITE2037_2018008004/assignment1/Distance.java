package assignment;

public class Distance {
	private String name;
	private double lat;
	private double lng;
	
	public Distance(String _name, double _lat, double _lng) {
		name = _name;
		lat = _lat;
		lng = _lng;
	}
	
	public String writeDistance() {
		return ( "Country : " + name + "\r\n" +
				 "latitude = " + lat + "\r\n" +
				 "longitude = " + lng + "\r\n" +
				 "--------------------\r\n");
	}
	
	public static String getDistance(Distance a, Distance b) {
		double _lat = a.getLat() - b.getLat();
		double _lng = a.getLng() - b.getLng();
		double dist = Math.sqrt(_lat*_lat + _lng*_lng);
		
		return ("Distance of\r\n" + 
				a.writeDistance() +
				b.writeDistance() +
				"is\r\n" +
				dist + "\r\n");
	}

	public double getLat() {
		return lat;
	}

	public double getLng() {
		return lng;
	}
}
