# Rose Online

## Setup
### Requirements
- Visual studio 2010 Premium/Professional/Ultimate SP1
- Visual studio 2017 Community (Optional)
- SQL Server Express 2014+
- SQL Server Management Studio

### Build
- Clone the repository
- Open the `All.sln` file
    - NOTE: If using VS2017 then **DO NOT** convert the solution or project files. You must have the VS2010 toolchain to compile this project.
- Build the solution

## Client
### Debugging
After installing the game client to the `game/` directory the `client` project can be run from within Visual Studio to debug the executable.

### Connecting to servers
Run `trose.exe @TRIGGER_SOFT _server <IP>` to connect to a server at a specific IP Address

## Server
### Build
- Open the `All.sln` file and build all `sho_*` projects

### Database
- Install SQL Server
- Enabled TCP/IP connection is SQL Server Configuration manager and ensure that the database is listening on the default port of `1433`
- Create 4 new databases `seven_ORA`, `SHO`, `SHO_LOG` and `SHO_MALL` with default settings
- Created 2 names user logins: `seven` and `sho`
    - User `seven` is required and the password is hardcoded into the login server. Set the password to: `tpqmsgkcm` or update the password in the login server lib (`sho_ls_lib/sho_ls_lib.cpp`)
    - User `sho` can have any password (recommended: `sho` password for dev)
- Run the 4 scripts from the `database/` directory (`seven_ora.sql`, `sho.sql`, `sho_log.sql`, `sho_mall.sql`)
- Enable `SQL Server and Windows Authentication` on your sql server (`Properties->Security`)
- Create 4 ODBC connections for each database using the `sho` user credentials. Ensure you are using the native client drivers.

### Configuration
- Run the `scripts/server_data.bat` script
- Update the server configuration files:

#### SHO_LS.ini
```
[FormLSCFG]
EditWaitTIME=1          // Delay before starting login server
EditDBIP=127.0.0.1      // Set this to our database IP
EditClientPORT=29000    // The external port to listen for client requests
EditServerPORT=19000    // The port to listen for server requests (i.e world and game server)
EditLoginRIGHT=0        // Filter users based on permissions, 0 is no filter
EditLimitUserCNT=0      // User count, 0 is no limit
EditGumsIP=127.0.0.1    // *Deprecated*
EditGumsPORT=20000      // *Deprecated*
CheckBoxGUMS=0          // *Deprecated*
CheckBoxWS=0            // *Deprecated*

EditLoginAccountDBName = SEVEN_ORA  
EditLoginAccountDBUser = sa         // Database login user that has access to seven_ORA database
EditLoginAccountDBPassword = sa     // Password

EditLogDBName = SHO_LOG             // Log database
EditLogDBUser = sa                  // Database login user that has access to SHO_LOG
EditLogDBPassword = sa              // Database login user that has access to SHO_LOG
EditPW=sa                           // Password for `SHO` user
```

#### SHO_WS.ini
```
[FormDataDIR]
EditDataDIR=C:\dev\rose\server\data  // Full path to the server data folder
EditWaitTIME=1                              // Delay before starting world server
ComboBoxLANG=8                              // *Deprecated* (Selects language)
item_0ComboBoxLANG=Korean
item_1ComboBoxLANG=English
item_2ComboBoxLANG=Japanese
item_3ComboBoxLANG=Chinese
item_4ComboBoxLANG=Reserved1
item_5ComboBoxLANG=Reserved2
item_6ComboBoxLANG=Reserved3
item_7ComboBoxLANG=Reserved4
selIdx_ComboBoxLANG=0
[FormWSCFG]
EditZoneListenPORT=19001                    // The port to listen for server requests (i.e login and game server)
EditWaitTIME=1                              // Delay before starting world server
EditLoginServerIP=127.0.0.1                 // IP for the login server (use external IP if hosting for non-local players)
EditLoginServerPORT=19000                   // Port for the login server, must match SHO_LS.ini
EditUserListenPORT=29100                    // Port to listen for client requests
EditExtIP=127.0.0.1                         // External IP address (make sure to set this so others can join)
EditWorldNAME=1Rose-local                  // World name, number is prepended to sort the names on the list
EditDBServerIP=127.0.0.1                    // Database IP
EditDBTableNAME=SHO
EditDBAccount=sho
EditDBPW=sho
EditLogAccount=sho
EditLogPW=sho
CheckBoxCreateCHAR=0                        // Enable/disable character creation
```

#### SHO_GS.ini
```
[FormDataDIR]
EditDataDIR=C:\dev\rose\server\data  // Full path to the server data folder
EditWaitTIME=1                              // Delay before starting game server
ComboBoxLANG=9                              // *Deprecated* (Selects language)
item_0ComboBoxLANG=Korean
item_1ComboBoxLANG=English
item_2ComboBoxLANG=Japanese
item_3ComboBoxLANG=Taiwanese
item_4ComboBoxLANG=Chinese
item_5ComboBoxLANG=Reserved1
item_6ComboBoxLANG=Reserved2
item_7ComboBoxLANG=Reserved3
item_8ComboBoxLANG=Reserved4
selIdx_ComboBoxLANG=1
[FormDBCFG]
EditWaitTIME=1
EditDBIP=127.0.0.1                          // Database IP address
EditDBName=SHO
EditDBUser=sho
EditDBPassword=sho
EditLogPassword=sho
EditLogUser=sho
EditMallUser=sho
EditMallPW=sho
EditWorldServerIP=127.0.0.1                 // World server ip address (use external ip if hosting for others)
EditLoginWorldPORT=19001                    // Login server port, must match `sho_ls.ini`
EditAccountServerIP=127.0.0.1               // **Deprecated**
EditAccountServerPORT=19002
[FormGSCFG]
EditWorldNAME=Test                          // World name
EditClientPORT=29200                        // Port to listen on for client requests
EditClientIP=127.0.0.1                      // External ip address
EditLowAGE=0
EditHighAGE=0
EditChannelNO=3
EditMaxUSER=100
ListViewZONE=29
```

### New user
Insert a row into `dbo.UserInfo`, the following fields are required:
- `Account` - The user's login name
- `Email` - The user's email address
- `MD5PassWord` - MD5 encoded version of user's password
- `MailIsConfirm` - Must be true for user to log in
- `Right` - The user permissions (0 - cant login, 1 - default, 768 - full admin)

### Start
After building everything launch `1-login.bat` then `2-world.bat` then `3-game.bat` and finally `local.bat`