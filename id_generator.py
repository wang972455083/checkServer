
import random

def validate(start, end):
	if(start < 0):
		start = 0
	if(end < 0):
		end = 0

	if(start > end):
		start, end = end, start

	return (start, end)

def generate_user_id(filePath, start, end):

	ids = [i for i in range(start, end)]
	random.shuffle(ids)
	try:
		with open(filePath, "w") as f:
			size = len(ids)
			for i in range(size):
				f.write(str(ids[i]))
				if(i < size - 1):
					f.write("\n")
	except IOError as err:
		print("File error" + str(err))


if __name__ == '__main__':
#'/var/lib/mysql-files/randID.txt'
	path = raw_input("please enter output file path:")
	start = int(raw_input("please enter start integer:"))
	end = int(raw_input("please enter end integer:"))
	start, end = validate(start, end)
	generate_user_id(path, start, end)

	print "finished generating user id from {0} to {1}".format(start, end)
	
	
	

# load data low_priority infile '/var/lib/mysql-files/randID.txt' into table user_id_table(UserId);
# delete from user_id_table where userid in (select Id from user);
# select * from user_id_table limit 15;