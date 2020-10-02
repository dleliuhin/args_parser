>   The main purpose of the implementation of this module is to make the life of a developer easier when writing a microservice taking into account the requirements for the code, as well as unifying the code for fast putting the application into operation.


# Abstract
Since a double nesting level is applied to the program parameters, i.e. the name of the configuration file is taken from the command line arguments, it is decided that no other arguments other than those described below are expected on the command line. Customize as much as you like through the configuration. The class takes it upon itself to check that users have not hammered any gag into the arguments.

## Flags

* -h, --help -- displays standard help, with a description passed to the constructor;
* --vgit, --vgit-hash, --vgit-author, --vgit-revcount, --vgit-email, etc -- deduces
build commit information;
* --print-conf -- outputs the config (default) passed to the constructor;
* -c filename, -cfilename, --config=filename -- takes the name of the configuration file;
* -p filename, -pfilename, --pid=filename -- takes filename for advisory blocking
and saving the pid of the process;

Any other flags are considered invalid. They will be printed and the program will end.

### --help
No comment.

### --vgit*
Works according to the principle described in the corresponding module from VLib. If there is such flag, the corresponding information will be displayed and the program will end.

### --print-conf
Prints what will come out of the vsettings passed to the constructor. It is assumed that the programmer passes the default settings.

### --config=filename
Takes in the name of the configuration file.

NB! Look carefully at the point about the V_DEVELOP macro !!!
If there is no macro, V_DEVELOP is a required argument!

The vsettings class is immediately set on it and replaces all occurrences `$$APP_NAME,
$$APP_PATH, $$FULL_APP` to the corresponding substitutions.

 - autocorrect affects all values of all subgroups in vsettings.

 - autocorrect runs in random order:
        - $$V_APP_PATH -- replaced with the path to the application;
        - $$V_APP_NAME -- replaced with the name of the application;
        - $$V_FULL_APP -- replaced with path / name;

NB! Replacement rules only apply if reading vsettings is assigned to arguments.
There is no magic if you decide that the module will read your mind and do autocorrect in both parties in the most unexpected places, then no. Autocorrect occurs if used method `niias :: arguments :: settings ()`.


### --pid=filename
Takes a filename to store the pid of the process.
It makes the so-called advisory blocking. If two processes are simultaneously capture the same file, the second process will not receive it. Respectively, will print the name of the offending process and exit.

Optional argument.


##API
The class API is as lightweight as possible. Everything that is possible is tucked away under the hood.

```
class niias::arguments
{
    arguments( int argc, char const * const * const argv,
               const std::string & app_description,
               const vsettings &   default_settings );

    ~arguments();

    std::string app_name() const;
    std::string app_path() const;
    std::string full_app() const;

    vsettings settings() const;
}
```

Pass the arguments from `main (argc, argv)` to the constructor, as a description
`app_description` is awaiting a service briefing. NB! No need to sculpt arguments into the briefing,
it itself! You just need to give a short description of the service.

Pass the settings to `default_settings`. I highly recommend using for assembly
`vsettings :: schema`. The following will be an example - a fish.

Methods:
```
    app_name(); //  the name of the running binary.
    app_path(); //  the path to it.
    full_app(); //  path with name.

     // Return the settings read from the file specified in the "-c" argument.
     // Further, see about the development flag !!!
    settings(); 
```


## Development flag ```V_DEVELOP```

Build systems don't have a consistent way to figure out how a project is built. Therefore,
I took the liberty of using the `V_DEVELOP` macro. If this macro is set,
then it is believed that the developer is currently working on the code and it is not convenient for him to use
any flags. Therefore, if there is no reference to the configuration file, `settings ()` will return
default settings. If the macro is not defined and there is no "-c" in the arguments, then
an error will be thrown.

## Example

```
//=======================================================================================
//  Settings example
//=======================================================================================
struct Settings
{
    struct ZCM
    {
        std::string target  = "ipc";
        std::string channel = "channel";
    } zcm;

    struct Logs
    {
        bool need_cout = true;

        bool need_shared = true;
        std::string shared_name = "$$FULL_APP.log";     // For autoreplace

        bool need_leveled = true;
        std::string leveled_path = "$$APP_PATH/logs";   // For autoreplace

        uint file_sizes = 1e6;
        uint rotates = 3;

        void setup()
        {
            if ( !need_cout )
                vlog::clear_executers();

            if ( need_shared )
                vlog::set_shared_log( shared_name, file_sizes, rotates );

            if ( need_leveled )
                vlog::set_leveled_log( leveled_path, file_sizes, rotates );
        }
    } logs;

    Settings()
    {
        _schema.subgroup( "ZCM" );
        _schema.add( "target",  &zcm.target  );
        _schema.add( "channel", &zcm.channel );
        _schema.end_subgroup();

        _schema.subgroup( "LOGS" );
        _schema.add( "need_cout",    &logs.need_cout    );

        _schema.add( "need_shared",  &logs.need_shared  );
        _schema.add( "shared_name",  &logs.shared_name  );

        _schema.add( "need_leveled", &logs.need_leveled );
        _schema.add( "leveled_path", &logs.leveled_path );

        _schema.add( "file_sizes", &logs.file_sizes );
        _schema.add( "rotates",    &logs.rotates    );
    }

    void capture( const vsettings& sett )
    {
        _schema.capture( sett );
    }

    static vsettings by_default()
    {
        return Settings()._schema.build();
    }

private:
    vsettings::schema _schema;
};
//=======================================================================================
//  Settings example
//=======================================================================================


//=======================================================================================
int main(int argc, char *argv[])
{
    niias::arguments args( argc, argv, "Example", Settings::by_default() );
    Settings settings;
    settings.capture( args.settings() );
    settings.logs.setup();
    ...
}
//=======================================================================================
```
