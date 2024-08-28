/*
 * ACS_CS_Utils.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: eanform
 */

#include "ACS_CS_Util.h"
#include <unistd.h>
#include <pwd.h>
#include <grp.h>


namespace ACS_CS_NS {

ACS_CS_Util::~ACS_CS_Util() {

}

bool ACS_CS_Util::setRecursivePermission(const std::string &path, perm permissions)
{
        bool result = true;

        fs::perms perms = (fs::perms)permissions;
        fs::path pathToSet(path);

        if (fs::exists(pathToSet))
        {
                try
                {
                        //set permission on first input file/folder
                        fs::permissions(pathToSet, perms);
                }
                catch (const std::exception & ex)
                {
                        result = false;
                        std::cout << ex.what() << std::endl;
                }

                if (result && fs::is_directory(pathToSet))
                {
                        fs::recursive_directory_iterator itr(pathToSet);
                        while (itr != fs::recursive_directory_iterator())
                        {
                                try
                                {
                                        if ( fs::is_directory(itr->status()) || fs::is_regular_file(itr->status()))
                                        {
                                                //set permission on iterated file/folder
                                                fs::permissions(itr->path(), perms);
                                        }
                                        else
                                        {
                                                result = false;
                                        }
                                }
				catch (const std::exception & ex)
                                {
                                        result = false;
                                        std::cout << itr->path().string() << " " << ex.what() << std::endl;
                                }


                                ++itr;
                        }
                }
        }

        return result;

}


bool ACS_CS_Util::setRecursivePermission(const std::string &path, perm p_dir_permissions, perm p_file_permissions, const std::string &owner, const std::string &group)
{
	bool result = true;
	uid_t _uid = -1;
	gid_t _gid = -1;		

	fs::perms perms_file = (fs::perms)p_file_permissions;
	fs::perms perms_dir = (fs::perms)p_dir_permissions;
	fs::path pathToSet(path);

	if (fs::exists(pathToSet))
	{
		if(owner.length() !=0) {
			struct passwd* owner_info = getpwnam(owner.c_str());
			if(owner_info) {
				_uid = owner_info->pw_uid;
			}
		}
		if(group.length() !=0) {
			struct group* group_info = getgrnam(group.c_str());
			if(group_info) {
				_gid = group_info->gr_gid;
			}
		}

		try
		{
			//set permission on first input file/folder
			fs::permissions(pathToSet, perms_dir);
			chown(path.c_str(), _uid, _gid);
		}
		catch (const std::exception & ex)
		{
			result = false;
			std::cout << ex.what() << std::endl;
		}

		if (result && fs::is_directory(pathToSet))
		{
			fs::recursive_directory_iterator itr(pathToSet);
			while (itr != fs::recursive_directory_iterator())
			{
				try
				{
					if ( fs::is_directory(itr->status()))
					{
						//set permission on iterated file/folder
						fs::permissions(itr->path(), perms_dir);
						chown(itr->path().c_str(), _uid, _gid);
					}
					else if ( fs::is_regular_file(itr->status()))
					{
						fs::permissions(itr->path(), perms_file);
						chown(itr->path().c_str(), _uid, _gid);
					}
					else
					{
						result = false;
					}
				}
				catch (const std::exception & ex)
				{
					result = false;
					std::cout << itr->path().string() << " " << ex.what() << std::endl;
				}


				++itr;
			}
		}
	}

	return result;

}

void ACS_CS_Util::splitString(std::string &first, std::string & second, const std::string & input, const std::string & separator)
{
	size_t pos = input.find_first_of(separator);
	if (pos != std::string::npos)
	{
		first =input.substr(0, pos);
		second = input.substr(pos + 1);
	}
}

}//namespace
