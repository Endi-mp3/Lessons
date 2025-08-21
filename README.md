# About

Very basic C lessons and homework tasks.

## Git

Create new local git in some project directory:
    
    `git init`

Add remote repository 

    `git remote add origin <git@somerepoaddress>`

When we have already some remote repository:

    `git clone <git@somerepoaddress>`

Show status

    `git status`

Add new file for tracking or add file to save changes in commit

    `git add <filename> <filename2> <directoryname>`

Add new files from current directory:

    `git add .`

Create new commit with editor window:

    `git commit`

Create new commit with message:

    `git commit -m "Commit message"`

Create new commit with message and for add already tracked files (doesn't affect on new files):

    `git commit -am 'message'`

Send new commits from local machine to remote git server:

    `git push origin <branch>`

    Main branch

    `git push origin main`


Get new commits from remote machine to local git repository:

    `git pull`

    If concreate branch required:

    `git pull origin <branchname>`

# Build


# Tests

## Fibonacci

    > Lessons % cd tests/additional/fibonacci
    > fibonacci % ./test.sh fib.txt <path to fibonacci binary>
