import sublime, sublime_plugin
from subprocess import Popen, PIPE, call, STDOUT
import sys, time

state = {}

def is_server_running(quick=False):
    #return True
    address = ('127.0.0.1', state["settings"].get("port"))  
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    try:
        client.connect(address)
    except:
        return False
    client.close()
    return True

def start_server():
    global state
    if not is_server_running():
        # clean up any old instances that may be running...
        stop_server()
        state["server"] = Popen(state["server_command"], shell=True)

import json,socket
import time

def recvall(sock, buffer_size=4096):
    buf = sock.recv(buffer_size)
    while buf:
        yield buf
        if not buf: break
        buf = sock.recv(buffer_size)

def stop_server():    
    if "server" in state and state["server"].poll() == None:
        address = ('127.0.0.1', state["settings"].get("port"))  
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
        try:
            client.connect(address)
            client.send(b'shutdown\n')
            client.recv(1024)
            client.close()
        except:
            pass
        if state["server"].poll() == None:
            state["server"].terminate()

def create_completion(completion):
    (name, completion_type) = completion.split(":")
    completion_type = completion_type.strip()
    completion = name
    # add the '(' for functions!
    if completion_type.startswith("function"):
        completion = name + "("

        # it's a Lua func and params have been found
        # if "|" in completion_type
        # doing this for the speed!
        if len(completion_type) >= 10:
            # split out the function params
            params = completion_type[11:].split()

            # set the completion type to just the start
            completion_type = completion_type[0:9] + "()"

            # figure this thing out
            completion = completion + ", ".join([ "${{{num}:{name}}}".format(num=num+1, name=val) for (num, val) in enumerate(params)])
            completion = completion + ")"
        
        # for c funcs, we can't do completion
        else:
            completion = completion + "$1)"


    return "{0}\t{1}".format(name, completion_type), completion


class LuaComplete(sublime_plugin.EventListener):    
    def on_query_completions(self, view, prefix, locations):
        #tic = time.time()       
        
        position = locations[0]
        scopes = view.scope_name(position).split()

        if ('source.lua' not in scopes or state["enabled"] == False):
            return None

        # load the server if it's not running.
        if not is_server_running():
            start_server()

        # we can only autocomplete certain things
        current_char = view.substr(position-1)
        if not state["settings"].get('readline') and current_char not in [":", ".", "[", "("]:
            return None

        # build the main command
        #command = "{client} -i -c {pos}".format(client=state["client_command"], pos=str(position))
        
        # append the filename if it exists
        file_name = view.file_name()
        #if file_name is not None:
        #    command = command + " -f '{0}'".format(file_name)

        # get all the window vars
        package_folders = []
        window_vars = view.window().extract_variables()
        if "folder" in window_vars:
            package_folders.append(window_vars["folder"])

        if state["additional_includes"]:
            package_folders.append(state["additional_includes"])

        # did we find a folder to add?
        #if package_folders:
        #    command = command + " -r '{0}'".format(';'.join(package_folders))


        # get the file contents
        file_contents = view.substr(sublime.Region(0, view.size()))
        #file_contents = file_contents.encode('utf8')

        # send it to the client
        # print(command)
        #client = Popen(command, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT)
        # print(file_contents)
        # print(position)

        # send communicate on stdin to the client
        #output = client.communicate(file_contents)[0].decode('utf-8')
        #output = output.splitlines()
        # print("returncode", client.returncode)

        request = {
            'filename' : file_name,
            'src' : file_contents,
            'cursor' : position,
            'packagePath' : ';'.join(package_folders)
        }

        address = ('127.0.0.1', state["settings"].get("port"))  
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
        client.connect(address)
        client.send((json.dumps(request) + '\n').encode('utf-8'))
        response = b''.join(recvall(client)).decode('utf-8')
        client.close()
        response = json.loads(response)
        output = [response['type']]
        if response['type'] == 'function':
            output += response['info']
        elif response['type'] == 'table':
            for v in response['info']:
                if v['type'] == 'function' and 'paramList' in v:
                    output.append('%s: %s | %s' % (v['name'], v['type'], ' '.join(v['paramList'])))
                else:
                    output.append('%s: %s' % (v['name'], v['type']))

        #print('time :',time.time() - tic)
        if True:#client.returncode == 0:
            view.set_status("a", "")
            output_type = output[0]
            # print(output_type)
            # main output is on lines 1 and below
            output = output[1:]
            # print(output)
            if output_type == "table":
                return [ create_completion(x) for x in output ]

        else:
            view.set_status("a", "The lua-complete client failed to return")
            # potentially retry the command or restart the server if his happens.

    def __exit__(self, type, value, traceback):
        stop_server()

# start and stop are really only used for debug
class StartServerCommand(sublime_plugin.ApplicationCommand):
    def run(self):
        start_server()

class StopServerCommand(sublime_plugin.ApplicationCommand):
    def run(self):
        stop_server()

class ClearCacheCommand(sublime_plugin.ApplicationCommand):
    def run(self):
        stop_server()
        start_server()

class DisableCommand(sublime_plugin.ApplicationCommand):
    def run(self):
        global state
        state["enabled"] = False

class EnableCommand(sublime_plugin.ApplicationCommand):
    def run(self):
        global state
        state["enabled"] = True

def plugin_loaded():
    global state
    state["settings"] = sublime.load_settings("LuaComplete.sublime-settings")

    # strip out the path/port
    path = state["settings"].get("path")
    if path is None:
        path = "lua-complete"
    port = state["settings"].get("port")
    if port is None:
        port = 51371

    # figure out if it's enabled
    enabled = state["settings"].get("enabled")
    if enabled is None:
        enabled = True

    # setup the command.
    state["server_command"] = "{path} server -p {port}".format(path=path, port=port)
    state["client_command"] = "{path} client -p {port}".format(path=path, port=port)
    state["enabled"] = enabled

    # get any additional include locations
    state["additional_includes"] = state["settings"].get("additional_includes")
