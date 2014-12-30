#ifndef CMD_H_
#define CMD_H_

#ifdef _cplusplus
extern "C" {
#endif

extern int cmd_cat(int argc, char *argv[]);
extern int cmd_ls(int argc, char *argv[]);
extern int cmd_mount(int argc, char *argv[]);
extern int cmd_unmount(int argc, char *argv[]);
extern int cmd_mem(int argc, char *argv[]);
extern int cmd_threads(int argc, char *argv[]);
extern int cmd_test(int argc, char *argv[]);

#ifdef _cplusplus
}
#endif

#endif /* CMD_H_ */
