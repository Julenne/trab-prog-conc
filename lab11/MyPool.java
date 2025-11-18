/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;

//-------------------------------------------------------------------------------
//!!! Documentar essa classe !!!

// Classe responsável por criar a pool de threads
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;


    //Cria as t
    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;

        // fila de tarefas runnable para serem executadas
        queue = new LinkedList<Runnable>(); 

        //cria as threads e inicia cada uma delas
        threads = new MyPoolThreads[nThreads]; 
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    //método que adiciona mais uma tarefa na fila, caso não esteja em shutdown(pool fechado)
    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    //método que fecha a pool de threads
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            //notifica todas as tarefas que estão em wait()
            queue.notifyAll();
        }
        //aguarda as threads terminarem com o join() e sai do método
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
            //enquanto a fila de tarefas estiver vazia e nao tiver shutdown ,
            //espera-se novas tarefas
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             //se shutdown == true e a fila estiver vazia, a thread termina,
             //senao remove a primeira tarefa da fila 
             if (queue.isEmpty()) return;
             r = (Runnable) queue.removeFirst(); // r recebe o objeto Runnable removido (a tarefa)
           }
           //executa a tarefa
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}

class Primo implements Runnable {
   
   private int n;
   //construtor
   Primo(int num) {
     this.n = num;
   }
   //...completar implementacao, recebe um numero inteiro positivo e imprime se esse numero eh primo ou nao
   public void run() {
    int i;
    if(n<=1){
      System.out.println(n + " nao e primo");
      return;
    } 
    if(n==2){
      System.out.println(n + " e primo");
      return;
    } 
    if(n%2==0){
      System.out.println(n + " nao e primo");
      return;
    } 
    for(i=3; i< Math.sqrt(n)+1; i+=2) {
      if(n%i==0){
        System.out.println(n + " nao e primo");
        return;
      } 
      
    }
    System.out.println(n + " e primo");
   }
}

//Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 50;

    public static void main (String[] args) {
      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (int i = 0; i < 20000; i++) {
        final String m = "Hello da tarefa " + i;
        //Runnable hello = new Hello(m);
        //pool.execute(hello);
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");
   }
}
