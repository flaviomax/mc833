import java.sql.*;

public class querybanco {

	/**
	 * @param args
	 * @throws SQLException 
	 * @throws ClassNotFoundException 
	 */
	public static void main(String[] args) throws SQLException, ClassNotFoundException {
		
		connectToAndQueryDatabase("root", "crislinda");

	}

	public static void connectToAndQueryDatabase(String username, String password) throws SQLException, ClassNotFoundException {
 
		try {
			Class.forName("com.mysql.jdbc.Driver");
		} catch (ClassNotFoundException e) {
			System.out.println("Where is your MySQL JDBC Driver?");
			e.printStackTrace();
			return;
		}
		
		Connection con = null;
	    
	    con = DriverManager.getConnection(
	                         "jdbc:mysql://localhost:3306/MC833",
	                         username,
	                         password);

	    Statement stmt = con.createStatement();
	    ResultSet rs = stmt.executeQuery("SELECT titulo, lancamento FROM Locadora");
	    
	    int columnsNumber = rs.getMetaData().getColumnCount();	  
	    
	    while (rs.next()) {
	        String s="";
	        for (int c=1; c <= columnsNumber; c++)
	        	s=s.concat(rs.getString(c)).concat(" ");
	        System.out.println(s);
	    }
	    
	    con.close();
	}
}
