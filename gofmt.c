#include <yed/plugin.h>

void
buffer_go_fmt();
void
ev_buffer_go_fmt(yed_event* event);
void
ev_reload_go(yed_event* event);

char bufferLoc[512];

int
yed_plugin_boot(yed_plugin* self)
{
    /*Check for plug version*/
    YED_PLUG_VERSION_CHECK();

    yed_event_handler gofmt;
    yed_event_handler reload;

    gofmt.kind = EVENT_BUFFER_PRE_WRITE;
    gofmt.fn   = ev_buffer_go_fmt;
    reload.kind = EVENT_BUFFER_POST_WRITE;
    reload.fn   = ev_reload_go;

    if (yed_get_var("gofmt-auto") == NULL)
    {
        yed_set_var("gofmt-auto", "yes");
    }

    if (yed_var_is_truthy("gofmt-auto"))
    {
        yed_plugin_add_event_handler(self, gofmt);
        yed_plugin_add_event_handler(self, reload);
    }

    yed_plugin_set_command(self, "go-fmt", buffer_go_fmt);
    return 0;
}

void
buff_path_for_fmt()
{
    yed_buffer* buffer;
    yed_frame*  frame;

    if (!ys->active_frame)
    {
        yed_cerr("no active frame");
        return;
    }

    frame = ys->active_frame;

    if (!frame->buffer)
    {
        yed_cerr("active frame has no buffer");
        return;
    }

    buffer = frame->buffer;

    if (buffer->name)
    {
        strcpy(bufferLoc, buffer->path);
    }
    else
    {
        yed_cerr("buffer has no path");
    }
}

int
status;

void
go_fmt(void)
{
    int        output_len;
    char       cmd_buff[1024];

    buff_path_for_fmt();
    snprintf(cmd_buff, sizeof(cmd_buff), "gofmt -w %s &> /tmp/golog", bufferLoc);
    LOG_CMD_ENTER("gofmt");
    yed_cerr("%s", yed_run_subproc(cmd_buff, &output_len, &status));
    LOG_EXIT();

    if (status != 0)
    {
        yed_cerr("Failure to format golang\n");
    }
    else
    {
        yed_cprint("Formatted buffer\n");
    }
}
void
ev_buffer_go_fmt(yed_event* event)
{
    yed_frame* frame;

    if (!ys->active_frame)
    {
        yed_cerr("no active frame");
        return;
    }

    frame = ys->active_frame;

    if (!frame->buffer)
    {
        yed_cerr("active frame has no buffer");
        return;
    }

    if (frame->buffer->ft == yed_get_ft("Golang"))
    {
        go_fmt();
    }
}
void
buffer_go_fmt()
{
    yed_frame* frame;

    if (!ys->active_frame)
    {
        yed_cerr("no active frame");
        return;
    }

    frame = ys->active_frame;

    if (!frame->buffer)
    {
        yed_cerr("active frame has no buffer");
        return;
    }

    if (frame->buffer->ft == yed_get_ft("Golang"))
    {
        go_fmt();
    }
}

void
ev_reload_go(yed_event* event)
{
    yed_frame* frame;

    if (!ys->active_frame)
    {
        yed_cerr("no active frame");
        return;
    }

    frame = ys->active_frame;

    if (!frame->buffer)
    {
        yed_cerr("active frame has no buffer");
        return;
    }

    if (frame->buffer->ft == yed_get_ft("Golang") && status == 0)
    {
        YEXE("buffer-reload");
        yed_cprint("Reloaded buffer\n");
    }
}
