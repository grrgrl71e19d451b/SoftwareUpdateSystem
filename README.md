# **Relazione sul Sistema di Gestione dei Pacchetti Software**

**1. Introduzione**

Il progetto descritto in questa relazione riguarda lo sviluppo di un sistema client-server per la gestione dei pacchetti software. Questo sistema consente il caricamento (upload) e lo scaricamento (download) di pacchetti tra client e server. Ogni pacchetto è identificato da un nome e una versione specifici. Il sistema è progettato per gestire richieste simultanee, garantendo la coerenza e l'integrità dei dati attraverso tecniche di sincronizzazione basate su blocchi a livello di file e un logging dettagliato implementato in un modulo comune.

**2. Componenti Principali**

Il sistema si compone di quattro componenti principali:

- **Server**: Gestisce tutte le richieste dei client per il caricamento e il download dei pacchetti. È responsabile della memorizzazione e della gestione delle versioni dei pacchetti, garantendo l'accesso sicuro e simultaneo ai dati utilizzando meccanismi di sincronizzazione basati su blocchi di file.
- **Client Upload**: Consente agli utenti di caricare pacchetti sul server. Ogni pacchetto caricato è accompagnato da un nome e una versione specifici, e l'operazione è loggata per garantirne la tracciabilità.
- **Client Download**: Permette agli utenti di scaricare pacchetti dal server. L'utente specifica il nome e la versione del pacchetto desiderato, e il client scarica il pacchetto corrispondente, registrando il risultato nel log per una documentazione accurata.
- **Common**: Garantisce coerenza e riusabilità tra server e client, includendo funzioni per il confronto delle versioni e per l'estrazione di nome e versione dai file.

**3. Dettagli del Server**

Il server è implementato per ascoltare le richieste dei client sulla porta 8080. Utilizza un approccio multi-processo tramite la system call fork, che consente di gestire contemporaneamente più connessioni client. Per garantire la coerenza dei pacchetti, il server utilizza meccanismi di sincronizzazione basati su blocchi a livello di file tramite la system call flock. Questo garantisce che le operazioni di lettura e scrittura sui pacchetti siano sicure e prive di condizioni di gara.

**System Call Utilizzate:**

- `socket`: Crea un endpoint di comunicazione per le connessioni di rete.
- `bind`: Associa l'endpoint a un indirizzo IP e a una porta specifici.
- `listen`: Configura il socket per ricevere connessioni in ingresso.
- `accept`: Accetta una connessione da un client.
- `read` e `write`: Leggono e scrivono dati dai/sui socket.
- `fork`: Crea un nuovo processo per gestire ogni client.
- `close`: Chiude la connessione una volta completata l'operazione.

**Funzioni Principali:**

- `handle_client`: Gestisce ogni connessione client, elaborando le richieste di upload o download e rispondendo di conseguenza. Utilizza funzioni esterne per il confronto delle versioni (`compare_versions`) e per il logging delle operazioni (`log_operation`, `log_existing_package`).
- `create_directory_if_not_exists`: Verifica l'esistenza delle directory necessarie per la memorizzazione dei pacchetti e le crea se necessario.

**4. Dettagli del Client Upload**

Il client upload è progettato per caricare pacchetti sul server, leggendo i comandi da un file di istruzioni. Una volta stabilita la connessione con il server, il client invia il pacchetto specificato e attende la conferma dell'operazione. Tutti i risultati, inclusi eventuali errori o successi, vengono registrati in un file di log per garantire una tracciabilità completa.

**System Call Utilizzate:**

- `socket`: Crea un endpoint di comunicazione per interagire con il server.
- `connect`: Stabilisce una connessione con il server.
- `write`: Invia i dati del pacchetto al server.
- `read`: Riceve la risposta dal server sull'esito dell'operazione di upload.
- `close`: Chiude la connessione una volta completato l'upload.

**Funzioni Principali:**

- `upload_package`: Gestisce l'intero processo di caricamento del pacchetto, dall'estrazione del nome e della versione dal filepath all'invio dei dati al server. Include un controllo rigoroso degli errori e il logging dell'operazione.
- `process_upload_instructions`: Legge e interpreta i comandi di upload da un file di istruzioni e li esegue in sequenza.

**5. Dettagli del Client Download**

Il client download funziona in maniera simile al client upload, ma è progettato per richiedere e scaricare pacchetti dal server. Legge i comandi da un file di istruzioni, stabilisce una connessione con il server, richiede il pacchetto desiderato e salva il file ricevuto localmente nel percorso specificato. Anche in questo caso, tutti i risultati delle operazioni, inclusi eventuali errori o successi, vengono registrati in un file di log per garantire una tracciabilità completa.

**System Call Utilizzate:**

- `socket`: Crea un endpoint di comunicazione per interagire con il server.
- `connect`: Stabilisce una connessione con il server.
- `write`: Invia la richiesta di download al server.
- `read`: Riceve il pacchetto dal server.
- `close`: Chiude la connessione dopo il completamento del download.

**Funzioni Principali:**

- `download_package`: Gestisce il processo di download del pacchetto dal server, richiedendo il file e salvandolo localmente. Include un controllo rigoroso degli errori e il logging dell'operazione.
- `process_download_instructions`: Legge e interpreta i comandi di download da un file di istruzioni e li esegue in sequenza.

**6. Strutture e Funzioni Comuni**

Le strutture e le funzioni comuni sono fondamentali per garantire un funzionamento coerente del sistema. La struttura Package rappresenta un pacchetto software e include informazioni come nome, versione, dimensione e contenuto. Le funzioni compare_versions e extract_name_and_version, definite nel file common.c, assicurano una gestione coerente dei pacchetti e delle versioni tra server e client, migliorando la modularità e la manutenibilità del codice.

**7. Logging**

Il logging è una componente essenziale del sistema, permettendo di tracciare tutte le operazioni eseguite sia dal server che dai client. Le funzionalità di logging sono implementate nel file `logger.c`, il quale fornisce funzioni per registrare eventi significativi in file di log specifici.

**Dettagli del Logging:**

- **Server Logs**: Ogni operazione eseguita dal server, sia essa un upload o un download, è registrata nel file `logs/server_log.log`, con dettagli come l'IP del client, la porta, l'operazione eseguita e il risultato. La funzione `log_operation` registra le operazioni standard, mentre `log_existing_package` annota i casi in cui esiste già una versione più recente del pacchetto richiesto.
- **Client Upload Logs**: Le operazioni di upload effettuate dai client sono registrate in un file di log dedicato, con informazioni dettagliate su ogni trasferimento e su eventuali errori.
- **Client Download Logs**: Analogamente, le operazioni di download sono registrate in un file di log, documentando il successo o il fallimento di ogni richiesta.
  
Il modulo logger.c fornisce la funzione log_to_file, che consente sia al server che ai client di registrare i log su file in modo sicuro e uniforme.

**8. Makefile**

Il progetto include un Makefile che automatizza il processo di compilazione e gestione del progetto. Il Makefile definisce i passi necessari per compilare il server e i client, organizzando i file sorgente, oggetto e binari nelle rispettive directory.

**Struttura del Makefile:**

- **Compilazione**: Il Makefile specifica come devono essere compilati i vari componenti del sistema, utilizzando `gcc` come compilatore e includendo flag per abilitare i messaggi di avvertimento e le informazioni di debug.
- **Pulizia**: Include un target `clean` per rimuovere i file compilati e i log generati, facilitando la gestione del progetto durante lo sviluppo.
- **Esecuzione**: Sono definiti target per avviare il server e i client, semplificando il processo di test ed esecuzione del sistema.

**9. Conclusione**

Il sistema di gestione dei pacchetti software è un'architettura client-server affidabile, che grazie alla gestione concorrente, alla sincronizzazione e alla robustezza, garantisce operazioni sicure di upload e download. L'integrazione delle funzioni esterne `common.c` e `logger.c` nel server e nei client migliora la modularità del codice e facilita la manutenzione. Grazie al logging esteso, il sistema permette un monitoraggio preciso delle operazioni, rendendo più semplice la diagnosi di eventuali problemi.
