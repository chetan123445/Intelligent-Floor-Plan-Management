                                             		INTELLIGENT FLOOR PLAN MANAGEMENT
							=================================

--> This project is about making or arranging new rooms and for booking them also.
--> It has several different files which includes:
	1:authentication.c++ : This files will ask for users to register or login and store the data in files as plain_users.txt and hashed_users.txt
	2:adminregistration.c++ : This files will register the admins and stored them in plain_admins.txt and hashed_admins.txt	
	3:floorplan.c++ : This file will first check that the user who is trying to access is an admin or not because an admin can only make changes for a room.
			  After registration, admin will ask to upload a new room, or modify an existing room, or view existing rooms details.
			  Upload includes a new room generation(create a new room by entering the required details).
			  Modify means changing some of its details like capacity or availability
	4:offlinemechanism.c++ : This file provides a way to store room details if we are offline.
				 It stores the details in an offline.txt file and after coming online, it will synchronize them.
	5:meetingroom.c++ : This file provides room booking mechanism.
			    If the room is suitable according to the capacity entered by the user, it will suggest all the rooms within that capacity and if available.
			    User can release the room after his/her work get completed.
			    User can view room details such as capacity, availability, last time booking and all that.
--> Generated files after all these C++ codes:
	1: floor_plans.txt : This will store all the rooms which will be created.
	2: floorname.txt : With regards to each floor there will be txt files of each floor which will have it's information.(like: room1.txt)
	3: plain_users.txt : This file will have username and there real passwords.
	4: hashed_users.txt : This file will have username and there hashed passwords for privacy concerns.
	5: plain_admins.txt : This file will have admin name and there real passwords.
	6: hashed_admins.txt : This file will have admin name and there hashed passwords for privacy concerns.
	7: offline.txt : This file will store all the floor which will be created when we select offline condition and once we come online, then all the data of this file will be transferred
                         to the floor_plans.txt 

