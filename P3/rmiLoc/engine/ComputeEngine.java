/*
 * Copyright (c) 1995, 2008, Oracle and/or its affiliates. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Oracle or the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */ 

package engine;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.sql.*;
import compute.Compute;

public class ComputeEngine implements Compute {

    public ComputeEngine() {
        super();
    }

    public String executeTask(String query) {
		try{
			//System.out.println(query);
			return connectToAndQueryDatabase("root", "crislinda", query);
		}
		catch (Exception e) {
			e.printStackTrace();
			return("Erro na execucao da query");
		}
    }

    public static void main(String[] args) {
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }
        
        try {
            String name = "Compute";
            Compute engine = new ComputeEngine();
            Compute stub =
                (Compute) UnicastRemoteObject.exportObject(engine, 0);
            Registry registry = LocateRegistry.getRegistry();
            registry.rebind(name, stub);
            System.out.println("ComputeEngine bound");
        } catch (Exception e) {
            System.err.println("ComputeEngine exception:");
            e.printStackTrace();
        }
    }
    
    public String connectToAndQueryDatabase(String username, String password, String query) throws SQLException, ClassNotFoundException {
 
		try {
			Class.forName("com.mysql.jdbc.Driver");
		} catch (ClassNotFoundException e) {
			System.out.println("Where is your MySQL JDBC Driver?");
			e.printStackTrace();
			return "ERROR";
		}
		
		Connection con = null;
	    //System.out.println("Ira conectar");
	    con = DriverManager.getConnection(
	                         "jdbc:mysql://localhost:3306/MC833",
	                         username,
	                         password);
	    String result = "";
	                         
		System.out.println("Conectou com o servidor");
		if (query.substring(0,6).equals("SELECT")){
			Statement stmt = con.createStatement();
			ResultSet rs = stmt.executeQuery(query);
			System.out.println("Buscou a query");
			int columnsNumber = rs.getMetaData().getColumnCount();	  
			
			while (rs.next()) {
				String s="";
				for (int c=1; c <= columnsNumber; c++)
					s=s.concat(rs.getString(c)).concat(" - ");
				result = result.concat(s).concat("\n");
			}
		}
		else { // se não é SELECT, é UPDATE
			try {
				PreparedStatement preparedStmt = con.prepareStatement(query);
				preparedStmt.executeUpdate();
				result = "Banco atualizado com sucesso!\n";				
			}
			catch (Exception e){
				result = "Erro na atualização do banco.\n";
			}
		}
	    
	    con.close();
	    return result;
	}
}
