cd `dirname $0`

# '.git'が存在するか
if [ ! -d .git ]; then
    echo "Not found .git directory. Please initialize git repository first."
    exit 1
fi

# git remote -vの結果にoneforallが含まれない場合
if ! git remote -v | grep -q 'oneforall_template' ; then
    echo "Not found remote repository 'oneforall_template'. Creating..."
    git remote add oneforall_template git@github.com:KeioRoboticsAssociation/oneforall_template.git
fi

# mainにpull
current_branch=$(git branch --show-current)
echo "Pulling from oneforall_template to $current_branch..."
ignore_files=(README.md ./oneforall_template.ioc ./${project_name}.ioc ./.install.sh ./.install.bat ./CMakeLists.txt)
git update-index --assume-unchanged ${ignore_files[@]}
git pull oneforall_template $current_branch --rebase
git update-index --no-assume-unchanged ${ignore_files[@]}