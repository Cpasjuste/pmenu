
#ifndef h_pnd_tinyxml_h
#define h_pnd_tinyxml_h

#ifdef __cplusplus
extern "C" {
#endif

unsigned char pnd_pxml_load ( const char* pFilename, pnd_pxml_t *app );
unsigned char pnd_pxml_parse ( const char *pFilename, char *buffer, unsigned int length, pnd_pxml_t *app );

#ifdef __cplusplus
}
#endif

#endif
