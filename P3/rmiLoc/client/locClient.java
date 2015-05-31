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

package client;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.math.BigDecimal;
import java.util.Scanner;
import java.lang.*;
import java.io.PrintWriter;
import compute.Compute;

public class locClient {
    public static void main(String args[]) {
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }
        try {
            String name = "Compute";
            Registry registry = LocateRegistry.getRegistry(args[0]);
            Compute comp = (Compute) registry.lookup(name);
            String query;
            query = select_query();
            int modo = select_modo();
            if (modo == 1){
				long before = System.nanoTime();
				String resultado = comp.executeTask(query);
				long after = System.nanoTime();
				System.out.println(resultado);
				System.out.println("Tempo de comunicacao: " + (after-before) + " ns");
			}
			else {
				PrintWriter writer = new PrintWriter ("clientResult.txt", "UTF-8");
				for (int i = 0; i < 30; i++){
					long before = System.nanoTime();
					String resultado = comp.executeTask(query);
					long after = System.nanoTime();
					writer.println((after-before));
				}
				writer.close();
			}
        } catch (Exception e) {
            System.err.println("locClient exception:");
            e.printStackTrace();
        }
    }
    
    private static int select_modo(){
		System.out.println ("\n*** Escolha o modo: ***");
		System.out.println ("0. 30 Vezes");
		System.out.println ("1. Teste");
		
		Scanner in = new Scanner (System.in);
		int modo = in.nextInt();
		if (modo != 0 && modo != 1) {
			System.out.println("Modo inexistente.");
			System.exit(1);
		}
		
		return modo;		
	}
    
    private static String select_query () {
		System.out.println ("\n*** Escolha que tipo de operacao deseja realizar: ***\n");
		System.out.println ("1. Listar todos os titulos e anos de lancamento");
		System.out.println ("2. Listar todos os titulos e anos de lancamento de um genero");
		System.out.println ("3. Dado o identificador de um filme, consultar a sinopse");
		System.out.println ("4. Dado o identificador de um filme, consultar tudo sobre ele");
		System.out.println ("5. Listar tudo de todos os filmes");
		System.out.println ("6. Alterar o numero de exemplares em estoque");
		System.out.println ("7. Dado o identificador de um filme, retornar o numero de exemplares em estoque");
		System.out.println ("\nOperacao: ");
		
		Scanner in = new Scanner (System.in);
		String op;
		String adendo;
		String query = "";
		op = in.nextLine();
		
		switch (op){
			case "1":
				query = "SELECT titulo, lancamento FROM Locadora";
				break;
				
			case "2":
				System.out.println ("Entre com o genero: ");
				adendo = in.nextLine();
				query = "SELECT titulo, lancamento FROM Locadora WHERE genero=\"" + adendo + "\"";
				break;
				
			case "3":
				System.out.println ("Entre com o identificador: ");
				adendo = in.nextLine();
				query ="SELECT sinopse FROM Locadora WHERE id=" + adendo;
				break;
				
			case "4":
				System.out.println ("Entre com o identificador: ");
				adendo = in.nextLine();
				query = "SELECT * FROM Locadora WHERE id=" + adendo;
				break;
				
			case "5":
				query = "SELECT * FROM Locadora";
				break;
				
			case "6":
				System.out.println ("Senha: ");
				adendo = in.nextLine();
				if (adendo.equals("1234")){
					query = "UPDATE Locadora SET exemplares=3 WHERE id=1";
					/* Esta opcao eh hard-coded para que nao haja inconsistencia
					 * entre o BD local e o remoto */
				}
				else{
					System.out.println ("Senha incorreta!");
					System.exit(1);
				}
				break;
				
			case "7":
				System.out.println ("Entre com o identificador: ");
				adendo = in.nextLine();
				query = "SELECT exemplares FROM Locadora WHERE id=" + adendo;
				break;
				
			case "8":
				System.out.println ("Erro proposital! Caso de testes.\n");
				query = "SELECT colunaErrada FROM Locadora";
				break;
				
			default:
				System.out.println ("Opcao nao disponivel!");
				System.exit(1);
		}
		
		return query;	
	}
}
