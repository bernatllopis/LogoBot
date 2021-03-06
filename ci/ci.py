#!/usr/bin/env python

# run the continuous integration process
# includes watching the git repo for pull requests and commits

import os
import sys
import requests
import json
from time import sleep, gmtime, strftime
from subprocess import call, check_output, CalledProcessError

repo_owner = 'snhack'
repo_name = 'LogoBot'

repos_rel_dir = '../../'
primary_repo_dir = 'LogoBot'
staging_repo_dir = primary_repo_dir + 'Staging'

ci_log_name = 'ci.log'
prhist = []

def dict_in_array(dict, key, value):
    res = False
    for item in dict:
        if item[key] == value:
            res = True
            exit
    return res

def poll(un, pw, proxies):
    
    cilog = open(ci_log_name, 'a+')
    cilog.seek(0)
    
    # load cilog into prhist
    lines = cilog.readlines()
    for line in lines:
        line = line.split('_')
        num = int(line[0])
        dt = line[1]
        if not dict_in_array(prhist, 'number', num):
            prhist.append({'number':num, 'updated_at':dt})

    print("Polling for pull requests for commits...")
    print("")
    
    while True:
        print(strftime("%H:%M:%S", gmtime()))
        print("Getting list of pull requests...")
        
        r = requests.get('https://api.github.com/repos/'+repo_owner+'/'+repo_name+'/pulls', auth=(un, pw), proxies=proxies)
        
        jso = r.json()
        
        print("  Found: "+str(len(jso))+" pull request(s)")
        
        for p in jso:
            print("Checking: #"+str(p['number']) + " - "+ p['title'] + " by "+p['user']['login'])
            """
            print(p['body'])
            print(p['state'])
            print(p['merged_at'])
            print(p['updated_at'])
            """
            
            # check if the pull request is ready to be merged
            if p['state'] == 'open' and p['merged_at']== None:
            
                # check if we've done it before?
                if not dict_in_array(prhist, 'number', p['number']):            
                    try:
                        errorlevel = 0
                        
                        oplog = 'Build Log\n---\n'
                        
                        comments_url = p['_links']['comments']['href']
                        
                        # comment
                        payload = {
                            'body':'CI: Starting build process...'
                        }
                        r = requests.post(comments_url, auth=(un, pw), proxies=proxies, data=json.dumps(payload))
                    
                        
                    
                        # Discard any local changes
                        print("  Clean working tree")
                        o = check_output(['git','reset'])
                        o += check_output(['git','checkout','--','.'])
                        o += check_output(['git','clean','-f'])
                        print(o)
                        oplog += "\nClean working tree\n"
                        oplog += o
                            
                        print("  Remote update")
                        o = check_output(['git','remote','update','-p'])
                        print(o)
                        oplog += "\nRemote Update\n"
                        oplog += o
                        
                        # rebase to master
                        print("  Merge fast-forward master")
                        o = check_output(['git','merge','--ff-only','master'])
                        print(o)
                        oplog += "\nMerge fast-forward\n"
                        oplog += o
            
                        branch = p['head']['ref']
                        
                        print("  Checkout master")
                        o = check_output(['git','checkout','master'])
                        print(o)
                        oplog += "\nCheckout master\n"
                        oplog += o
                        
                        print("  Merge branch: "+branch)
                        o = ''
                        try:
                            o = check_output(['git','merge','--strategy-option','theirs','--no-commit','origin/'+branch])
                            print(o)
                        except CalledProcessError as e:
                            print("  Error: "+ str(e.returncode))
                            errorlevel = e.returncode
                            
                        oplog += "\nMerge branch: origin/"+branch+"\n"
                        oplog += o
                        
                        if errorlevel == 0:                        
                            # Now run the build process    
                            print("  Building")
                        
                            os.chdir('hardware/ci')
                            o = ''
                            try:
                                o = check_output(['./build.py'])
                                print(o)
                            except CalledProcessError as e:
                                print("  Error: "+ str(e.returncode))
                                errorlevel = 1
                        
                            os.chdir('../../')
                            
                            oplog += "\n\nBuilding\n--------\n"
                            oplog += o
                
                
                        if errorlevel == 0:
                            print("  Passed, auto-merging into master...")
                            
                            # comment
                            payload = {
                                'body':'CI: Build process successful - auto-merging into master\n\n' + oplog
                            }
                            r = requests.post(comments_url, auth=(un, pw), proxies=proxies, data=json.dumps(payload))
                            
                            # merge
                            payload = {
                                'commit_message':p['title']
                            }
                            r = requests.put(p['_links']['self']['href'] + '/merge', 
                                              auth=(un, pw), proxies=proxies, data=json.dumps(payload))
                            print(r)
                        
                        else:
                            print("  Errors, adding to pull request comments...")
                            
                            # log the error
                            payload = {
                                'body':'CI: Unable to auto-merge, build process encountered errors\n\n' + oplog
                            }
                            r = requests.post(comments_url, auth=(un, pw), proxies=proxies, data=json.dumps(payload))
                            
                        
                
                        # Log this request so we don't process it again
                        hist = {'number':p['number'], 'updated_at':p['updated_at']}
                        prhist.append(hist)
                        cilog.write(str(p['number']) + '_' + p['updated_at'] + '_' + str(errorlevel)+'\n')
                        cilog.flush()
                        
                        print("  Done")
                
                
                    except CalledProcessError as e:
                        print("Error: "+ str(e.returncode))
                else:
                    print("  Skipping")
                    
            else:
                print("  Error: Pull request not open or already merged")
            
        print("")
        sleep(60)
        call(['clear'])
        
    cilog.close()
    

def ci(un, pw, http_proxy="", https_proxy=""):
    print("Continuous Integration")
    print("----------------------")
    
    proxies = {
      "http": http_proxy,
      "https": https_proxy,
    }
    
    print("")
    print("Checking connection to github...")
    
    r = requests.get('https://api.github.com/user', auth=(un, pw), proxies=proxies)
    if r.status_code == 200:
        print("  OK")
        
        print("Changing working directory...")
        os.chdir(repos_rel_dir)
        
        print("  Now in: "+os.getcwd())
        
        print("Changing to staging dir: "+staging_repo_dir)
        if os.path.isdir(staging_repo_dir):
            os.chdir(staging_repo_dir)
            print("  OK")
            
            # Could check for empty dir here and if so, do a git clone?
            # git clone git@github.com:snhack/LogoBot .
            
            contents = os.listdir('.')
            if len(contents) == 0:
                print("  Staging empty - cloning repo")
                o = check_output(['git','clone','git@github.com:'+repo_owner+'/'+repo_name,'.'])
            
            poll(un, pw, proxies)
        
        else:
            print("  Error: Staging dir does not exist")
        
    
    else:
        print("  Error")
        print("  Status Code: "+r.status_code)
        print("  Response: "+r.text)
    
    
    
    # o = check_output(['git','branch'])

if __name__ == '__main__':
    if len(sys.argv) == 3:
        ci(sys.argv[1], sys.argv[2], "", "")
    elif len(sys.argv) > 3:
        ci(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        print("Usage: ci <git-username> <git-password> <http_proxy> <https_proxy>")