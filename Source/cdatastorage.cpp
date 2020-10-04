//******************************************************************************
// Copyright 2020 ThirtySomething
//******************************************************************************
// This file is part of Sieve.
//
// Sieve is free software: you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// Sieve is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
// more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Sieve. If not, see <http://www.gnu.org/licenses/>.
//******************************************************************************

#include "cdatastorage.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

// *****************************************************************************
// Namespace of Sieve
// *****************************************************************************
namespace net
{
    // *****************************************************************************
    // Namespace of Sieve
    // *****************************************************************************
    namespace derpaul
    {
        // *****************************************************************************
        // Namespace of Sieve
        // *****************************************************************************
        namespace sieve
        {
            // *****************************************************************************
            // Constants
            // *****************************************************************************
            const long long CDataStorage::m_bitsize = 64L;

            // *****************************************************************************
            // *****************************************************************************
            CDataStorage::CDataStorage(long long sieveSize)
            {
                lldiv_t tmpSize = getStoragePosition(sieveSize);
                m_storageSize = tmpSize.quot + 1LL;
                clear();
            }

            // *****************************************************************************
            // *****************************************************************************
            void CDataStorage::clear(void)
            {
                m_storage = std::vector(m_storageSize, 0LL);
            }

            // *****************************************************************************
            // *****************************************************************************
            std::tuple<long long, long long> CDataStorage::dataLoad(std::string filename)
            {
                std::ifstream infile(filename);
                long long index, sieveElement, latestPrime, sieveSize;

                m_storage.clear();
                infile >> latestPrime;
                infile >> sieveSize;
                while (infile >> index >> sieveElement)
                {
                    m_storage[index] = sieveElement;
                }
                infile.close();

                return {latestPrime, sieveSize};
            }

            // *****************************************************************************
            // *****************************************************************************
            void CDataStorage::dataSave(std::string filename, long long latestPrime, long long sieveSize)
            {
                std::ofstream myfile;
                myfile.open(filename);

                myfile << latestPrime << std::endl;
                myfile << sieveSize << std::endl;

                for (long long index = 0; index < m_storageSize; index++)
                {
                    myfile << index << " " << m_storage[index] << std::endl;
                }

                myfile.close();
            }

            // *****************************************************************************
            // *****************************************************************************
            void CDataStorage::exportPrimes(std::string filename, long long latestPrime)
            {
                std::ofstream myfile;
                myfile.open(filename);

                for (long long currentPrime = 0LL; currentPrime < latestPrime; currentPrime++)
                {
                    if (!isNumberNotPrime(currentPrime))
                    {
                        myfile << currentPrime << std::endl;
                    }
                }
                myfile.close();
            }

            // *****************************************************************************
            // *****************************************************************************
            long long CDataStorage::findNextPrime(long long number)
            {
                long long startValue = number + 1;
                while (isNumberNotPrime(startValue))
                {
                    startValue++;
                }
                return startValue;
            }

            // *****************************************************************************
            // *****************************************************************************
            long long CDataStorage::getStorageSize(void)
            {
                return m_storageSize;
            }

            // *****************************************************************************
            // *****************************************************************************
            long long* CDataStorage::getStoragePointer(void)
            {
                return m_storage.data();
            }

            // *****************************************************************************
            // *****************************************************************************
            bool CDataStorage::isNumberNotPrime(long long number)
            {
                lldiv_t internalPosition = getStoragePosition(number);
                long long element = m_storage[internalPosition.quot];
                bool result = (element >> internalPosition.rem) & 1LL;
                return result;
            }

            // *****************************************************************************
            // *****************************************************************************
            void CDataStorage::markNumberAsNotPrime(long long number)
            {
                lldiv_t internalPosition = getStoragePosition(number);
                long long element = m_storage[internalPosition.quot];
                element |= 1LL << internalPosition.rem;
                m_storage[internalPosition.quot] = element;
            }

            // *****************************************************************************
            // *****************************************************************************
            lldiv_t CDataStorage::getStoragePosition(long long number)
            {
                lldiv_t result = lldiv(number, CDataStorage::m_bitsize);
                return result;
            }
        } // namespace sieve
    }     // namespace derpaul
} // namespace net
