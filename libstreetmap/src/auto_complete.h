#ifndef AUTO_COMPLETE_H
#define AUTO_COMPLETE_H

void auto_complete();
char** command_completion(const char* stem_text, int start, int end);
char* intersection_name_generator(const char* stem_text, int state);

#endif /* AUTO_COMPLETE_H */

