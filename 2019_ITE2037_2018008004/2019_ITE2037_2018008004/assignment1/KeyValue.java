package assignment;

import java.util.StringTokenizer;

public class KeyValue {
	private String key;
	private String value;
	
	public KeyValue(String str) {
		StringTokenizer st = new StringTokenizer(str,"=");
		key = st.nextToken();
		value = st.nextToken();
	}
	
	public KeyValue(String str1, String str2) {
		key = str1;
		value = str2;
	}
	
	public String getKey() {
		return key;
	}

	public String getValue() {
		return value;
	}	
}
