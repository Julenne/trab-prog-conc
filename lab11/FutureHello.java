/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;

//classe runnable
class MyCallable implements Callable<Long> {
  // construtor
  MyCallable() {
  }

  // método para execução
  public Long call() throws Exception {
    long s = 0;
    for (long i = 1; i <= 100; i++) {
      s++;
    }
    return s;
  }
}

class Primo implements Callable<Integer> {

  private long n;

  // construtor
  Primo(long num) {
    this.n = num;
  }

  private int ehPrimo(long n) {
    if (n <= 1)
      return 0;
    if (n == 2)
      return 1;
    if (n % 2 == 0)
      return 0;

    for (long i = 3; i <= Math.sqrt(n); i += 2) {
      if (n % i == 0)
        return 0;
    }
    return 1;
  }

  @Override
  public Integer call() throws Exception {
    return ehPrimo(n);
  }
}

// classe do método main
public class FutureHello {
  private static final int N = 3;
  private static final int NTHREADS = 10;

  public static void main(String[] args) {
    // cria um pool de threads (NTHREADS)
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    // cria uma lista para armazenar referencias de chamadas assincronas
    List<Future<?>> list = new ArrayList<>();

    // for (int i = 0; i < N; i++) {
    //   Callable<Long> worker = new MyCallable();
    //   /*
    //    * submit() permite enviar tarefas Callable ou Runnable e obter um objeto Future
    //    * para acompanhar o progresso e recuperar o resultado da tarefa
    //    */
    //   Future<Long> submit = executor.submit(worker);
    //   list.add(submit);
    // }

    // System.out.println(list.size());

    // pode fazer outras tarefas...

    long num = 67;
    for (long i = 1; i < num; i++) {
      Callable<Integer> worker = new Primo(i);
      Future<Integer> submit = executor.submit(worker);
      list.add(submit);
    }

    // recupera os resultados e faz o somatório final
    long sum = 0;
    for (Future<?> future : list) {
      try {
        Object result = future.get(); // retorna Object e bloqueia se a computação nao tiver terminado
        if (result instanceof Long) {
          sum += (Long) result;
        } else if (result instanceof Integer) {
          sum += (Integer) result;
        }
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    System.out.println("entrada= "+ num + "; nthreads=" + NTHREADS + "; saida=" + sum );
    executor.shutdown();
  }
}
