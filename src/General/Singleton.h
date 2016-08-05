/**
 * Copyright (C) 2016 Martin Ubl <http://kennny.cz>
 *
 * This file is part of BubbleWorld MMORPG engine
 *
 * BubbleWorld is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BubbleWorld is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BubbleWorld. If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef BW_SINGLETON_H
#define BW_SINGLETON_H

/*
 * Singleton template class used for all singletons in application
 */
template<class T>
class Singleton
{
    public:
        // retrieve singleton instance
        static T* getInstance()
        {
            // if does not exist, create new
            if (!m_instance)
                m_instance = new T;

            return m_instance;
        }

    private:
        // only instance
        static T* m_instance;
};

template<class T> T* Singleton<T>::m_instance = nullptr;

#endif
