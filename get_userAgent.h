//
// Created by alessandro on 01/12/16.
//

#ifndef WEBSERVER_GET_USERAGENT_H
#define WEBSERVER_GET_USERAGENT_H


char *parsingGetHTTP(char str[]){
    return strtok(str, "\n");
}
char *parsingUserAgentHTTP(char str[]){
    char *get = strtok(str, "\n");
    char *accept = strtok(NULL, "\n");
    char *accept_lenguage = strtok(NULL, "\n");
    char *referer = strtok(NULL, "\n");
    return strtok(NULL, "\n");
}






char *get (char str[]) {
    char *GET = parsingGetHTTP(str);
    char *get2 = strtok (GET, " ");
    char *get3 = strtok (NULL, " ");
    char *get4 = strtok (NULL, " ");
    char *get5 = strtok (NULL, " ");

    if (!get5)
        get5 = "";

    printf ("get2: %s\n" "get3: %s\n" "get4: %s\n" "get5: %s\n", get2, get3, get4, get5);

    return get3;

}


char *userAgent(char str[]){
     char *USERAGENT = parsingUserAgentHTTP(str);

    char *user_agent2 = strtok (USERAGENT, " ");
    char *user_agent3 = strtok (NULL, "");
    char *user_agent4 = strtok (NULL, " ");

    if (!user_agent4)
        user_agent4 = "";

    printf ("user_agent2: %s\n" "user_agent3: %s\n" "user_agent4: %s\n", user_agent2, user_agent3, user_agent4);

    return user_agent3;

 }


#endif //WEBSERVER_GET_USERAGENT_H
