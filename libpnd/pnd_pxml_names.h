#ifndef h_pnd_pxml_names_h
#define h_pnd_pxml_names_h

#ifdef __cplusplus
extern "C" {
#endif

/* <PXML id="..."> ...*/
#define PND_PXML_ATTRNAME_UID "id"

/* <application id="123">*/
#define PND_PXML_APP "application"

/* <title lang="..."> */
#define PND_PXML_ENAME_TITLE "title"
#define PND_PXML_ATTRNAME_TITLELANG "lang"

/* <description lang="...">... */
#define PND_PXML_ENAME_DESCRIPTION "description"
#define PND_PXML_ATTRNAME_DESCRLANG "lang"

/* <exec background="true" standalone="false" command="..." startdir="..." /> */
#define PND_PXML_ENAME_EXEC "exec"
#define PND_PXML_ATTRNAME_EXECBG "background"
#define PND_PXML_ATTRNAME_EXECSTAL "standalone"
#define PND_PXML_ATTRNAME_EXECCMD "command"
#define PND_PXML_ATTRNAME_EXECARGS "arguments"
#define PND_PXML_ATTRNAME_EXECWD "startdir"
#define PND_PXML_ATTRNAME_EXECNOX11 "no_x11"

/* <icon src="..." /> */
#define PND_PXML_ENAME_ICON "icon"
#define PND_PXML_ATTRNAME_ICONSRC "src"

/* <previewpics>
 *   <pic src="..." />
 * </previewpics>
 */
#define PND_PXML_NODENAME_PREVPICS "previewpics"
#define PND_PXML_ENAME_PREVPIC "pic"
#define PND_PXML_ATTRNAME_PREVPICSRC "src"

/* <author name="..." website="..." email="..." /> */
#define PND_PXML_ENAME_AUTHOR "author"
#define PND_PXML_ATTRNAME_AUTHORNAME "name"
#define PND_PXML_ATTRNAME_AUTHORWWW "website"
#define PND_PXML_ATTRNAME_AUTHOREMAIL "email"

/* <version major="1" minor="1" release="1" build="2" /> */
#define PND_PXML_ENAME_VERSION "version"
#define PND_PXML_ATTRNAME_VERMAJOR "major"
#define PND_PXML_ATTRNAME_VERMINOR "minor"
#define PND_PXML_ATTRNAME_VERREL "release"
#define PND_PXML_ATTRNAME_VERBUILD "build"

/* <osversion major="1" minor="0" release="0" build="0" /> */
#define PND_PXML_ENAME_OSVERSION "osversion"
#define PND_PXML_ATTRNAME_OSVERMAJOR "major"
#define PND_PXML_ATTRNAME_OSVERMINOR "minor"
#define PND_PXML_ATTRNAME_OSVERREL "release"
#define PND_PXML_ATTRNAME_OSVERBUILD "build"

/* <categories>
 *   <category name="...">
 *     <subcategory name="..." />
 *   </category>
 * </categories>
 */
#define PND_PXML_NODENAME_CATS "categories"
#define PND_PXML_ENAME_CAT "category"
#define PND_PXML_ATTRNAME_CATNAME "name"
#define PND_PXML_ENAME_SUBCAT "subcategory"
#define PND_PXML_ATTRNAME_SUBCATNAME "name"

/* <associations>
 *   <association name="CSS File" filetype="text/css" exec="-f %s -t css" />
 * </associations>
 */
#define PND_PXML_NODENAME_ASSOCS "associations"
#define PND_PXML_ENAME_ASSOC "association"
#define PND_PXML_ATTRNAME_ASSOCNAME "name"
#define PND_PXML_ATTRNAME_ASSOCFTYPE "filetype"
#define PND_PXML_ATTRNAME_ASSOCARGS "arguments"

/* <clockspeed frequency="600" /> */
#define PND_PXML_ENAME_CLOCK "clockspeed"
#define PND_PXML_ATTRNAME_CLOCKFREQ "frequency"

/* <package name="foo" released="1/1/0001"/> */
#define PND_PXML_ENAME_PACKAGE "package"
#define PND_PXML_ATTRNAME_PACKAGE_NAME "name"
#define PND_PXML_ATTRNAME_PACKAGE_DATE "released"

/* <mkdir>
 *   <dir path="..." />
 * </mkdir>
 */
#define PND_PXML_NODENAME_MKDIR "mkdir"
#define PND_PXML_ENAME_MKDIR "dir"
#define PND_PXML_ATTRNAME_MKDIRPATH "path"

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* ifndef h_pnd_pxml_names_h */
