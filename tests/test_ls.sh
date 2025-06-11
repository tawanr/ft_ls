#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Function to compare outputs
compare_outputs() {
    local test_name="$1"
    local args="$2"
    
    echo "Running test: $test_name"
    
    valgrind --leak-check=full -s --log-file=leak_output.txt ./ft_ls $args > our_output.txt

    script -q -c "/usr/bin/ls $args" system_output.txt > /dev/null
    sed -i -e 's/\/usr\/bin\/ls/.\/ft_ls/g' system_output.txt
    
    sed -i '1d' system_output.txt
    sed -i '$d' system_output.txt
    sed -i '$d' system_output.txt
    
    if diff -w our_output.txt system_output.txt > /dev/null; then
        echo -e "${GREEN}✓ Test passed${NC}"
        if grep -q "in use at exit: 0 bytes in 0 blocks" leak_output.txt; then
            echo -e "${GREEN}✓ Leak check passed${NC}"
        else
            echo -e "${RED}✗ Leak check failed${NC}"
            cat leak_output.txt
        fi
    else
        echo -e "${RED}✗ Test failed${NC}"
        echo "Differences found:"
        diff -w our_output.txt system_output.txt
    fi
    echo "----------------------------------------"
}

# Create test directory structure
mkdir -p test_dir
sleep 1
current_time=$(date +%Y%m%d%H%M)
cd test_dir
for filename in "file1" "file2" "file3"; do
    # touch $filename
    # sleep 1
    touch -t $current_time $filename
    current_time=$((current_time - 2))
done
chmod +x file1
for dirname in "dir1" "dir2" "dir1/dir3" "dir1/dir3/dir4" "dir1/dir3/dir4/dir5"; do
    mkdir -p $dirname
    sleep 1
done
for filename in "dir1/dir3/dir4/dir5/file6" "dir1/dir3/dir4/file7" "dir1/dir3/file8" "dir1/file4" "dir2/file5" ; do
    touch $filename
    sleep 1
done
touch -t 202402010830 old_file
ln -s file1 symlink1
cd ..

# Basic tests
echo "Running basic tests..."
compare_outputs "Basic listing" "test_dir"
compare_outputs "Long format" "-l test_dir"
compare_outputs "Recursive" "-R test_dir"
compare_outputs "Recursive with long format" "-lR test_dir"
compare_outputs "All files" "-a test_dir"
compare_outputs "Long format with all files" "-la test_dir"
compare_outputs "Multiple arguments" "test_dir/file1 test_dir/dir1"
compare_outputs "Multiple arguments" "test_dir/dir1 test_dir/file1"
compare_outputs "Multiple arguments in long format" "-la test_dir/dir1 test_dir/file1 test_dir/dir1/dir3/dir4/dir5/file6"
compare_outputs "Sort by time" "-t test_dir"
compare_outputs "Reverse sort" "-r test_dir"
compare_outputs "Sort by time reverse" "-tr test_dir"
compare_outputs "Non-existent directory" "test_dir/nonexistent"
compare_outputs "Non-existent directory mixed" "test_dir/nonexistent test_dir/file1 test_dir"
compare_outputs "Non-existent directory mixed list" "-l test_dir/nonexistent test_dir/file1 test_dir"
compare_outputs "List with access time" "-lu test_dir"
compare_outputs "List without sorting" "-f test_dir"
compare_outputs "List without owners" "-g test_dir"
compare_outputs "List without owners" "-lga test_dir"
compare_outputs "All flags" "-lRtra test_dir"

# Cleanup
rm -rf test_dir
rm our_output.txt system_output.txt leak_output.txt

echo "All tests completed!" 