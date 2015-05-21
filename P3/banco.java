import java.sql.*;


public class banco {
	
	public void connectToAndQueryDatabase(String username, String password) throws SQLException {

	    Connection con = DriverManager.getConnection(
	                         "jdbc:myDriver:myDatabase",
	                         username,
	                         password);

	    Statement stmt = con.createStatement();
	    ResultSet rs = stmt.executeQuery("SELECT titulo, lancamento FROM Locadora");
	    
	    int columnsNumber = rs.getMetaData().getColumnCount();	  
	    
	    while (rs.next()) {
	        String s="";
	        for (int c=0; c< columnsNumber; c++)
	        	s=s.concat(rs.getString(c));
	        System.out.println(s);
	    }
	}
}
