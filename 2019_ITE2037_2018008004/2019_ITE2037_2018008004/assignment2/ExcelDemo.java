package assignment2;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.table.DefaultTableCellRenderer;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileFilter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.StringTokenizer;

public class ExcelDemo extends JFrame implements ActionListener {
	//#########################FEILD####################
	private static int WIDTH = 610;
	private static int HEIGHT = 588;
	private JScrollPane scrollPane;
	private JTable table, headerTable;
	private JMenuBar menuBar;
	private JMenu fileMenu, formulasMenu, functionMenu;
	private JMenuItem newItem, open, save, exit, sum,
					  average, count, max, min;
	private String title;
	private int cardinality, degree;
	//#########################FEILD####################	
	
	//생성자
	public ExcelDemo () {	
		title = "새 Microsoft Excel 워크시트.xlsx - Excel"; //기본 제목
		
		setTitle(title); //제목 설정
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); //close누를시 종료
		setSize(WIDTH, HEIGHT); //frame 크기 설정
		setLocationRelativeTo(null);  //화면 중앙에 위치하게 하는 코드
		
		
		setLayout( new BorderLayout() ); //레이아웃 형식 : borderlayout
		String data1[][] = new String[100][26]; //table 채우기 
		String head1[] = new String[26]; //table header
		
		for( int i = 0 ; i < 26 ; ++i) {
			head1[i] = new String( Character.toString(('A' + i)) ); //table col header A to Z
		}
		
		headerTable = new JTable(100,1); // table, headertable 생성
		table = new JTable(data1, head1); 
		
		for( int i = 0 ; i < 100 ; ++i) {
			headerTable.setValueAt(Integer.toString(i), i, 0); // headertable에서 rowheader로 쓸 숫자 넣기
		}
		
		DefaultTableCellRenderer rr = new DefaultTableCellRenderer();
		rr.setHorizontalAlignment(SwingConstants.CENTER);
		headerTable.getColumnModel().getColumn(0).setCellRenderer(rr); // rowheader 숫자 가운데 정렬
		
		rr = new DefaultTableCellRenderer() {
			public Component getTableCellRendererComponent( JTable table, Object value, boolean isSelected,
					boolean hasFocus, int row, int col) {
				JComponent com = (JComponent)super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, col);
			
			if(isSelected)
				com.setFont(getFont().deriveFont(Font.BOLD)); // 강조!
			return com;
			}
		}; // cell 선택시 해당되는 rowheader를 강조
		
		headerTable.setDefaultRenderer(headerTable.getColumnClass(0), rr); //rowheader에 적용
		headerTable.setSelectionModel(table.getSelectionModel()); 
		
		headerTable.setBackground(new Color(243,243,243)); //rowheader 색칠
		headerTable.getColumnModel().getColumn(0).setPreferredWidth(60); //rowheader 너비 설정
		headerTable.setPreferredScrollableViewportSize(new Dimension(60,0));
		
		headerTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF); //headertable 크기 설정 불가
		table.setAutoResizeMode(JTable.AUTO_RESIZE_OFF); //talbe 크기 설정 불가
		
		scrollPane = new JScrollPane(table); //table을 scrollpane에 붙이기
		scrollPane.createHorizontalScrollBar(); //수평 스크롤바 붙이기
		
		scrollPane.setColumnHeaderView(table.getTableHeader()); //columntable을 table의 header로 설정
		scrollPane.setRowHeaderView(headerTable); //rowheader headertalbe로 설정
		add(scrollPane); //scrollpane frame에 붙이기
		
		createMenu();
		table.addMouseListener(new MouseAdapter() {	
			@Override
			public void mouseClicked(MouseEvent event) {
				cardinality = table.getSelectedRow();
				degree = table.getSelectedColumn();
			}
		}); //table 이벤트 처리
	}
	
	//createMenu 메쏘드
	public void createMenu () {
		menuBar = new JMenuBar();
		
		fileMenu = new JMenu("File"); //메뉴 이름 설정
		formulasMenu = new JMenu("Formulas");
		functionMenu = new JMenu("Function");
		
		newItem = new JMenuItem("New"); //메뉴아이템 이름 설정
		open = new JMenuItem("Open");
		save = new JMenuItem("Save");
		exit = new JMenuItem("Exit");
		
		sum = new JMenuItem("SUM");
		average = new JMenuItem("AVERAGE");
		count = new JMenuItem("COUNT");
		max = new JMenuItem("MAX");
		min = new JMenuItem("MIN");
		
		fileMenu.add(newItem); //filemenu에 filemenuItem 붙이기
		fileMenu.add(open);
		fileMenu.addSeparator(); //구분자
		fileMenu.add(save);
		fileMenu.addSeparator(); //구분자
		fileMenu.add(exit);
		
		functionMenu.add(sum); //functionMenu에 붙이기
		functionMenu.add(average);
		functionMenu.add(count);
		functionMenu.add(max);
		functionMenu.add(min);
		
		formulasMenu.add(functionMenu); //formulas에 functionMenu 붙이기
		
		menuBar.add(fileMenu); //메뉴바에 file,formulas메뉴 넣기
		menuBar.add(formulasMenu);
		
		setJMenuBar(menuBar); //JMenubar를 menuBar 설정
		
		sum.addActionListener(this); //MenuItem들을 ActionListener에 추가
		average.addActionListener(this);
		count.addActionListener(this);
		max.addActionListener(this);
		min.addActionListener(this);
		
		newItem.addActionListener(this);
		open.addActionListener(this);
		save.addActionListener(this);
		exit.addActionListener(this);
	}

	//event처리 메쏘드
	public void actionPerformed(ActionEvent e) {
		JFileChooser jfc; //open, save를 위한 filechooser
		BufferedReader br; //open에 사용될 BR
		BufferedWriter bw; //save에 사용될 BW
		String str, from, to, line; 
		StringTokenizer st, st1; //open save , formulas에 쓰일 ST
		int idx1,idx2;
		int idx3,idx4;
		int colcount, rowcount;
		
		//###########################################FUNCTION####################################
		if(e.getSource() == sum) {
			double _sum = 0;
			str = JOptionPane.showInputDialog(null,"Function Arguments", 
													"SUM", JOptionPane.PLAIN_MESSAGE); //input dialog출력
			st = new StringTokenizer(str,":"); //string받아 앞뒤 처리
			from = st.nextToken();
			to = st.nextToken();
			idx1 = Integer.parseInt(from.substring(1));
			idx2 = Integer.parseInt(to.substring(1));
			idx3 = from.charAt(0) - 'A';
			idx4 = to.charAt(0) - 'A';

			for( int i = idx1 < idx2 ? idx1 : idx2 ; i <= (idx1 < idx2 ? idx2 : idx1) ; ++i) { //앞뒤 위치를 바꾸어 써도 범위를 알맞게 잡도록 설정
				for( int j = idx3 < idx4 ? idx3 : idx4 ; j <= (idx3 < idx4 ? idx4 : idx3) ; ++j ) {
					if(table.getValueAt(i, j) != null && table.getValueAt(i, j).toString().trim().length() != 0) //nullpointer exception 처리
						_sum += Double.parseDouble(table.getValueAt(i, j).toString()); //범위에 있는 숫자들 모두 더하기
					}
				}
			
			table.setValueAt(Double.toString(_sum), cardinality, degree ); //click된 cell에 값 갱신
		}
		
		else if(e.getSource() == average) {
			double _average = 0;
			int count = 0;
			str = JOptionPane.showInputDialog(null,"Function Arguments", 
												   "AVERAGE", JOptionPane.PLAIN_MESSAGE);
			st = new StringTokenizer(str,":");
			from = st.nextToken();
			to = st.nextToken();
			idx1 = Integer.parseInt(from.substring(1));
			idx2 = Integer.parseInt(to.substring(1));
			idx3 = from.charAt(0) - 'A';
			idx4 = to.charAt(0) - 'A';

			for( int i = idx1 < idx2 ? idx1 : idx2 ; i <= (idx1 < idx2 ? idx2 : idx1) ; ++i) {
				for( int j = idx3 < idx4 ? idx3 : idx4 ; j <= (idx3 < idx4 ? idx4 : idx3) ; ++j ) {
					if(table.getValueAt(i, j) != null && table.getValueAt(i, j).toString().trim().length() != 0) {
						_average += Double.parseDouble(table.getValueAt(i, j).toString()); //범위 안에 숫자를 모두 더하고
						count++; //valid한 cell의 갯수 세기
						}
					}
				}
			table.setValueAt(Double.toString(_average/count), cardinality, degree ); //값 갱신
		}
		
		else if(e.getSource() == count) {
			int cnt = 0;
			str = JOptionPane.showInputDialog(null,"Function Arguments", 
													"COUNT", JOptionPane.PLAIN_MESSAGE);
			st = new StringTokenizer(str,":");
			from = st.nextToken();
			to = st.nextToken();
			idx1 = Integer.parseInt(from.substring(1));
			idx2 = Integer.parseInt(to.substring(1));
			idx3 = from.charAt(0) - 'A';
			idx4 = to.charAt(0) - 'A';

			for( int i = idx1 < idx2 ? idx1 : idx2 ; i <= (idx1 < idx2 ? idx2 : idx1) ; ++i) {
				for( int j = idx3 < idx4 ? idx3 : idx4 ; j <= (idx3 < idx4 ? idx4 : idx3) ; ++j ) {
					if(table.getValueAt(i, j) != null && table.getValueAt(i, j).toString().trim().length() != 0) 
						cnt++; //valid한 cell의 갯수 세기
						
					}
				}
			table.setValueAt(Integer.toString(cnt), cardinality, degree ); //정수형으로 값 갱신
		}
		
		else if(e.getSource() == max) {
			double _max = Double.MIN_VALUE, temp; //max를 가장 작은 값으로 설정
			str = JOptionPane.showInputDialog(null,"Function Arguments", 
													"MAX", JOptionPane.PLAIN_MESSAGE);
			st = new StringTokenizer(str,":");
			from = st.nextToken();
			to = st.nextToken();
			idx1 = Integer.parseInt(from.substring(1));
			idx2 = Integer.parseInt(to.substring(1));
			idx3 = from.charAt(0) - 'A';
			idx4 = to.charAt(0) - 'A';

			for( int i = idx1 < idx2 ? idx1 : idx2 ; i <= (idx1 < idx2 ? idx2 : idx1) ; ++i) {
				for( int j = idx3 < idx4 ? idx3 : idx4 ; j <= (idx3 < idx4 ? idx4 : idx3) ; ++j ) {
					if(table.getValueAt(i, j) != null && table.getValueAt(i, j).toString().trim().length() != 0) {
						temp = Double.parseDouble(table.getValueAt(i, j).toString());
						_max = _max > temp ? _max : temp; //최대값을 구한다
						}
					}
				}
			table.setValueAt(Double.toString(_max), cardinality, degree ); //값 갱신
		}
		
		else if(e.getSource() == min) {
			double _min = Double.MAX_VALUE, temp; //min을 최대로 설정
			str = JOptionPane.showInputDialog(null,"Function Arguments", 
													"MIN", JOptionPane.PLAIN_MESSAGE);
			st = new StringTokenizer(str,":");
			from = st.nextToken();
			to = st.nextToken();
			idx1 = Integer.parseInt(from.substring(1));
			idx2 = Integer.parseInt(to.substring(1));
			idx3 = from.charAt(0) - 'A';
			idx4 = to.charAt(0) - 'A';

			for( int i = idx1 < idx2 ? idx1 : idx2 ; i <= (idx1 < idx2 ? idx2 : idx1) ; ++i) {
				for( int j = idx3 < idx4 ? idx3 : idx4 ; j <= (idx3 < idx4 ? idx4 : idx3) ; ++j ) {
					if(table.getValueAt(i, j) != null && table.getValueAt(i, j).toString().trim().length() != 0) {
						temp = Double.parseDouble(table.getValueAt(i, j).toString());
						_min = _min < temp ? _min : temp;
						}
					}
				}
			table.setValueAt(Double.toString(_min), cardinality, degree );
		}
		
		//###########################################FUNCTION####################################
				
		//###########################################FILE####################################
		
		else if(e.getSource() == newItem) {
			this.dispose();
			new ExcelDemo().setVisible(true);
		}
		
		else if(e.getSource() == open) {
			jfc = new JFileChooser();
			jfc.setAcceptAllFileFilterUsed(false); //다른 종류의 파일 고르는걸 막음
			jfc.setFileFilter(new FileNameExtensionFilter(".csv", "csv")); //csv파일 허용
			jfc.addChoosableFileFilter(new FileNameExtensionFilter(".txt", "txt")); //txt파일 허용
			
			if(jfc.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) { //파일이 잘 열렸다면
				title = jfc.getSelectedFile().getAbsolutePath(); //절대경로로 타이틀 설정
				setTitle(title); 	
			
				for( int i = 0 ; i < table.getRowCount() ; ++i) {
					for ( int j = 0 ; j < table.getColumnCount() ; ++j ) {
						table.setValueAt( null, i, j); //table의 모든 값을 null로 초기화
					}
				}
			
				try {
					br = new BufferedReader(new FileReader(title)); //절대경로로 파일을 열고
					line = br.readLine(); //줄마다 받기
					rowcount = 0;
					colcount = 0;
					while(line != null && rowcount <= 99) { //
						st1 = new StringTokenizer(line, ",");
						//int tem = st1.countTokens();
						while(st1.hasMoreElements() && colcount <= 25 ) { //col갯수보다 적고, st의 element가 더 있는 상황에서
							String tmp = st1.nextToken();
							table.setValueAt(tmp, rowcount, colcount++); // table값을 갱신
						}
						colcount = 0; //한줄이 끝나면 colcount 0으로 초기화
						line = br.readLine(); //한줄을 더 받고
						rowcount++; //rowcount 한개 올리기
					}
					br.close(); //br을 다 쓰고 닫기.
				} catch (IOException e1) { //예외 처리
					e1.printStackTrace(); 
				}
				for( int i = 0 ; i < table.getRowCount() ; ++i) {
					for ( int j = 0 ; j < table.getColumnCount() ; ++j ) {
						if(table.getValueAt(i, j) != null && table.getValueAt(i, j).toString().equals(" ")) // 띄어쓰기를 모두 null로 바꿈
							table.setValueAt( null, i, j);
					}
				}
			}
		}
		
		else if(e.getSource() == save) {
			jfc = new JFileChooser();
			if (jfc.showSaveDialog(null) == JFileChooser.APPROVE_OPTION ) { //저장 경로를 잘 설정했다면
				try {
					bw = new BufferedWriter(new FileWriter(jfc.getSelectedFile())); //선택된 경로로 파일 쓰기
					for( int i = 0 ; i < 100 ; ++i ) {
						for( int j = 0 ; j < 26 ; ++j ) {
							if(table.getValueAt(i, j) != null && table.getValueAt(i, j).toString().trim().length() != 0) // 값이 있다면
								bw.write(table.getValueAt(i, j).toString()); //table 값으로 쓰기
							else
								bw.write(" "); //null값은 띄어쓰기로
							bw.write(","); // 구분자 표시
						}
						bw.newLine(); // 한줄 엔터쓰기
					}
					bw.close(); // 다 쓴뒤 bw 닫기
				}
				catch ( Exception e1 ) {
					e1.printStackTrace();
				}	
			}
		}
		
		else if(e.getSource() == exit) {
			System.exit(0); // 프로그램 종료
		}
		
		//###########################################FILE####################################
		
	}
		
	//main
	public static void main(String[] args) {
		new ExcelDemo().setVisible(true); //객체 생성, frame 보이게 하기.
	}
}
