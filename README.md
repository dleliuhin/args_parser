UPD Редакция от 13-07-2020, by elapidae.

> Данный репозиторий содержит реализацию требований к общему коду, описанных
[здесь](http://confluence.niias/pages/viewpage.action?pageId=24349153).

>   Основной целью реализации данного модуля является облегчение жизни разрабочика при
написании микросервиса с учетом требований к коду, а также унификации кода для быстрого
ввода приложения в эксплуатацию.


# Abstract
Так как к параметрам программы применяется двойной уровень вложенности, т.е. из
аргументов командной строки берется имя файла конфигурации, принято решение, что никаких
других аргументов, кроме описанных ниже, в командной строке не ожидается.
Настраивайте сколько душе угодно через конфигурацию. Класс берет на себя обязанность
проверить, что одмины не вбили в аргументы какой-нибудь отсебятины.

## Флаги

* -h, --help -- выводит стандартную помощь, с описанием, переданным в конструктор;
* --vgit, --vgit-hash, --vgit-author, --vgit-revcount, --vgit-email, etc -- выводит
информацию о коммите сборки;
* --print-conf -- выводит конфирурацию (по умолчанию), переданную в конструктор;
* -c filename, -cfilename, --config=filename -- принимает имя файла конфигурации;
* -p filename, -pfilename, --pid=filename -- принимает имя файла для advisory блокировки
и сохранении pid процесса;

Любые другие флаги воспринимаются как ошибочные. Они будут распечатаны и программа
завершится.

### --help
Без комментариев.

### --vgit*
Работает по принципу, описанному в соответствующем модуле из vlibs2. Если есть такой
флаг, то будет выведена соответствующая информация и программа завершится.

### --print-conf
Печатает что получится из переданного в конструктор vsettings. Подразумевается, что
программист передает настройки по умолчанию.

### --config=filename
Принимает имя файла конфигурации.

NB! См. внимательно пункт про макрос V_DEVELOP!!!
В случае отсутствия макроса V_DEVELOP является обязательным аргументом!

Сразу же на него натравливается класс vsettings и заменяет все вхождения `$$APP_NAME,
$$APP_PATH, $$FULL_APP` на соответствующие подстановки.
Это крайне полезно для гибкой настройки, например, путей логирования и пр. девупсных
штучек. Рекомендую.

 - автозамены действуют на все значения всех подгрупп в vsettings.

 - автозамены проходят в произвольном порядке:
        - $$V_APP_PATH -- заменяется на путь к приложению;
        - $$V_APP_NAME -- заменяется на имя приложения;
        - $$V_FULL_APP -- заменяется на путь/имя;

NB! Действуют правила замены только если чтение vsettings поручается arguments.
Магии нету, если вы решили, что модуль будет читать ваши мысли и делать автозамену в обе
стороны в самых неожиданных местах, то нет. Автозамена происходит если используется
метод `niias::arguments::settings()`.


### --pid=filename
Принимает имя файла для сохранения pid процесса.
Делает, так называемую, advisory блокировку. Если два процесса одновременно будут
захватывать один и тот же файл, то второй процесс ее не получит. Соответственно,
выведет имя процесса обидчика и завершится.

Необязательный аргумент.


##API
API класса максимально облегчено. Все что только возможно убрано под капот.

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

В конструктор передаем аргументы из `main(argc,argv)`, в качестве описания
`app_description` ожидается брифинг сервиса. NB! Не нужно в брифинг лепить аргументы,
оно само! Вам нужно просто дать краткое описание сервиса.

В `default_settings` передаем настройки. Крайне рекомендую использовать для сборки
`vsettings::schema`. Далее будет пример - рыбка.

Методы:
```
    app_name(); //  имя запущенного бинарника.
    app_path(); //  путь к нему.
    full_app(); //  путь с именем.

    //  Возвращают настройки, прочитанные из файла, указанного в аргументе "-c".
    //  Далее, см. про флаг разработки!!!
    settings(); 
```


##Флаг разработки `V_DEVELOP`

В системах сборки нет унифицированного способа выяснить как собирается проект. Поэтому,
я взял на себя смелость использовать макрос `V_DEVELOP`. Если этот макрос установлен,
то считается, что сейчас разработчик работает над кодом и ему не удобно использовать
какие-либо флаги. Поэтому, если нет указания на файл конфигурации, `settings()` вернет
настройки по умолчанию. Если же макрос не определен и в аргументах нету "-c", то
бросится ошибка.


##Пример

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
