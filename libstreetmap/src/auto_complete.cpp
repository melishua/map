#include "auto_complete.h"
#include "global.h"
#include <readline/readline.h>
#include <readline/history.h>

void auto_complete(){
    //Use tab for auto completion
        rl_bind_key('\t', rl_complete);

    //Use our function for auto-complete
    rl_attempted_completion_function = command_completion;
    //Tell readline to handle double and single quotes for us
    rl_completer_quote_characters = strdup("\"\'");

    char* buf; //Buffer of line entered by user
    while ((buf = readline("prompt> ")) != NULL) { //Prompt the user for input
        if (strcmp(buf, "") != 0)
            add_history(buf); //Only add to history if user input not empty

        //Currently we just print out the input line
        std::cout << buf << std::endl;

        //And exit if the user requested it
        if (strcmp(buf, "exit") == 0 || strcmp(buf, "quit") == 0) {
            break;
        }

        //readline generates a new buffer every time,
        //so we need to free the current one after we've finished
        free(buf);
        buf = NULL; //Mark it null to show we freed it
    }

    //If the buffer wasn't freed in the main loop we need to free it now
    // Note: if buf is NULL free does nothing
    free(buf);
}

//Given the stem `stem_text' perform auto completion.
//It returns an array of strings that are potential completions
//
//Note:
// `start' and `end' denote the location of `stem_text' in the global
// `rl_line_buffer' variable which contains the users current input line.
// If you need more context information to determine how to complete `stem_text'
// you could look at `rl_line_buffer'.

char** command_completion(const char* stem_text, int start, int end) {
    char ** matches = NULL;

    if (start != 0) {
        //Only generate completions if `stem_text'
        //is not the first thing in the buffer
        matches = rl_completion_matches(stem_text, intersection_name_generator);
    }

    return matches;
}

//Called repeatedly for a given `stem_text'. Each time it returns a potential
//match. When there are no more matches it returns NULL.
//
//The `state' variable is zero the first time it is called with a given
//`stem_text', and positive afterwards.

char* intersection_name_generator(const char* stem_text, int state) {
    //Static here means a variable's value persists across function invocations
    static int count;
    if (state == 0) {
        //We initialize the count the first time we are called
        //with this stem_text
        count = -1;
    }

    int text_len = strlen(stem_text);

    //Search through intersection_names until we find a match
    while (count < (int) mymap.street_name_list.size() - 1) {
        count++;
        if (strncmp(mymap.street_name_list[count].c_str(), stem_text, text_len) == 0) {
            //Must return a duplicate, Readline will handle
            //freeing this string itself.
            return strdup(mymap.street_name_list[count].c_str());
        }
    }

    //No more matches
    return NULL;
}
