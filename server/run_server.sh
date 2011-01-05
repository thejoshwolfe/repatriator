echo entering infinite loop
while [ 1 ]
do
    python $(dirname $0)/server.py
done

