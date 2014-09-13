//
//  Designer.h
//  Designer
//
//  Created by Wahid Tanner on 5/13/13.
//

#ifndef Designer_Designer_h
#define Designer_Designer_h

#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

namespace MuddledManaged
{
    namespace Designer
    {
        class VerificationException : public std::exception
        {
        public:
            VerificationException ()
            { }
            
            virtual const char * what () const noexcept
            {
                return mMessage.c_str();
            }
            
        protected:
            static std::string narrow (const std::wstring & wideString)
            {
                std::locale loc;
                std::vector<char> narrowCharBuf(wideString.size());
                std::use_facet<std::ctype<wchar_t>>(loc).narrow(
                    wideString.data(),
                    wideString.data() + wideString.size(),
                    '?',
                    narrowCharBuf.data());
                std::string narrowString = narrowCharBuf.data();
                return narrowString;
            }

            std::string mMessage;
        };
        
        class BoolVerificationException : public VerificationException
        {
        public:
            BoolVerificationException (bool expectedValue)
            : mExpectedValue(expectedValue)
            {
                initialize();
            }
            
            std::string expectedValue () const
            {
                if (mExpectedValue)
                {
                    return "true";
                }
                else
                {
                    return "false";
                }
            }
            
        protected:
            bool mExpectedValue;
            
        private:
            void initialize ()
            {
                mMessage = "    Bool verification failed.\n"
                           "        Expected: " + expectedValue() + "\n";
            }
        };
        
        class SameVerificationException : public VerificationException
        {
        public:
            SameVerificationException ()
            {
                initialize();
            }
            
        private:
            void initialize ()
            {
                mMessage = "    Verification that objects are the same failed.\n";
            }
        };
        
        class EqualVerificationException : public VerificationException
        {
        public:
            EqualVerificationException (bool expectedValue, bool actualValue)
            : mExpectedValue(std::to_string(expectedValue)), mActualValue(std::to_string(actualValue))
            {
                initialize();
            }
            
            EqualVerificationException (int expectedValue, int actualValue)
            : mExpectedValue(std::to_string(expectedValue)), mActualValue(std::to_string(actualValue))
            {
                initialize();
            }

            EqualVerificationException (long expectedValue, long actualValue)
            : mExpectedValue(std::to_string(expectedValue)), mActualValue(std::to_string(actualValue))
            {
                initialize();
            }

            EqualVerificationException (double expectedValue, double actualValue)
            : mExpectedValue(std::to_string(expectedValue)), mActualValue(std::to_string(actualValue))
            {
                initialize();
            }

            EqualVerificationException (const std::string & expectedValue, const std::string & actualValue)
            : mExpectedValue(expectedValue), mActualValue(actualValue)
            {
                initialize();
            }

            EqualVerificationException (const std::wstring & expectedValue, const std::wstring & actualValue)
            : mExpectedValue(narrow(expectedValue)),
              mActualValue(narrow(actualValue))
            {
                initialize();
            }
            
            std::string expectedValue () const
            {
                return mExpectedValue;
            }
            
            std::string actualValue () const
            {
                return mActualValue;
            }
            
        protected:
            std::string mExpectedValue;
            std::string mActualValue;
            
        private:
            void initialize ()
            {
                mMessage = "    Equal verification failed.\n"
                           "        Expected: " + expectedValue() + "\n"
                           "          Actual: " + actualValue() + "\n";
            }
        };
        
        class ScenarioBase
        {
        public:
            virtual ~ScenarioBase ()
            { }
            
            virtual std::string categoryFullName () const
            {
                return mCategoryFullName;
            }
            
            virtual std::string description () const
            {
                return mDescription;
            }
            
            virtual bool exceptionExpected () const
            {
                return mExceptionExpected;
            }
            
            virtual bool passed () const
            {
                return mRunPassed;
            }
            
            virtual void run ()
            {
                // Scenarios will pass unless one of the verify methods fail.
                mRunPassed = true;
                
                runSteps();
            }
            
            virtual void runSteps () = 0;
            
            virtual std::shared_ptr<ScenarioBase> clone () const = 0;

            virtual void verifyTrue (bool actualValue)
            {
                if (!actualValue)
                {
                    mRunPassed = false;
                    throw BoolVerificationException(true);
                }
            }
            
            virtual void verifyFalse (bool actualValue)
            {
                if (actualValue)
                {
                    mRunPassed = false;
                    throw BoolVerificationException(false);
                }
            }
            
            virtual void verifyEqual (bool expectedValue, bool actualValue)
            {
                if (actualValue != expectedValue)
                {
                    mRunPassed = false;
                    throw EqualVerificationException(expectedValue, actualValue);
                }
            }

            virtual void verifyEqual (int expectedValue, int actualValue)
            {
                if (actualValue != expectedValue)
                {
                    mRunPassed = false;
                    throw EqualVerificationException(expectedValue, actualValue);
                }
            }

            virtual void verifyEqual (long expectedValue, long actualValue)
            {
                if (actualValue != expectedValue)
                {
                    mRunPassed = false;
                    throw EqualVerificationException(expectedValue, actualValue);
                }
            }

            virtual void verifyEqual (double expectedValue, double actualValue)
            {
                if (actualValue != expectedValue)
                {
                    mRunPassed = false;
                    throw EqualVerificationException(expectedValue, actualValue);
                }
            }

            virtual void verifyEqual (const std::string & expectedValue, const std::string & actualValue)
            {
                if (actualValue != expectedValue)
                {
                    mRunPassed = false;
                    throw EqualVerificationException(expectedValue, actualValue);
                }
            }

            virtual void verifyEqual (const std::wstring & expectedValue, const std::wstring & actualValue)
            {
                if (actualValue != expectedValue)
                {
                    mRunPassed = false;
                    throw EqualVerificationException(expectedValue, actualValue);
                }
            }
            
            template <typename T>
            void verifySame (T * expectedValue, T * actualValue)
            {
                if (actualValue != expectedValue)
                {
                    mRunPassed = false;
                    throw SameVerificationException();
                }
            }
            
        protected:
            ScenarioBase (const std::string & categoryFullName, const std::string & scenarioDescription, bool exceptionExpected)
            : mCategoryFullName(categoryFullName), mDescription(scenarioDescription), mExceptionExpected(exceptionExpected)
            { }
            
            ScenarioBase (const ScenarioBase & src)
            : mCategoryFullName(src.mCategoryFullName), mDescription(src.mDescription), mExceptionExpected(src.mExceptionExpected)
            { }
            
        private:
            ScenarioBase & operator = (const ScenarioBase & rhs) = delete;
            
            std::string mCategoryFullName;
            std::string mDescription;
            bool mExceptionExpected;
            bool mRunPassed;
        };
        
        template <typename ExceptionT = std::exception>
        class Scenario : public ScenarioBase
        {
        public:
            Scenario (const std::string & categoryFullName, const std::string & scenarioDescription, bool exceptionExpected)
            : ScenarioBase(categoryFullName, scenarioDescription, exceptionExpected)
            { }
            
            virtual ~Scenario ()
            { }
            
        protected:
            Scenario (const Scenario & src)
            : ScenarioBase(src)
            { }
            
        private:
            Scenario & operator = (const Scenario & rhs) = delete;
        };
        
        class Category
        {
            friend class ScenarioManager;
            
        public:
            Category (const std::string & name)
            : mName(name)
            {
            }
            
            Category (const Category & src)
            : mName(src.mName), mChildCategories(src.mChildCategories), mChildScenarios(src.mChildScenarios)
            { }
            
            virtual ~Category ()
            { }
            
            virtual std::string name () const
            {
                return mName;
            }
            
            std::vector<std::shared_ptr<Category>> categories ()
            {
                return mChildCategories;
            }
            
            std::vector<std::shared_ptr<ScenarioBase>> scenarios ()
            {
                return mChildScenarios;
            }
            
            virtual std::shared_ptr<ScenarioBase> registerScenario (const ScenarioBase * scenario)
            {
                std::shared_ptr<ScenarioBase> sharedScenario(scenario->clone());
                
                mChildScenarios.push_back(sharedScenario);
                
                return sharedScenario;
            }
            
            virtual void run (std::ostream & stream)
            {
                for (auto & category : mChildCategories)
                {
                    category->run(stream);
                }
                
                stream << "---- Running scenarios in: " << name() << std::endl;
                for (auto & scenario : mChildScenarios)
                {
                    try
                    {
                        scenario->run();
                        if (scenario->passed())
                        {
                            stream << "Scenario passed: " <<
                                scenario->description() << std::endl;
                        }
                        else
                        {
                            stream << "Scenario failed: " <<
                                scenario->description() << std::endl;
                        }
                    }
                    catch (VerificationException ex)
                    {
                        stream << "Scenario failed: " <<
                            scenario->description() << std::endl <<
                            ex.what();
                        continue;
                    }
                    catch (...)
                    {
                        stream << "Scenario failed unexpectedly: " <<
                            scenario->description() << std::endl;
                        continue;
                    }
                }
            }
            
        private:
            Category & operator = (const Category & rhs) = delete;
            
            std::string mName;
            // We can use shared_ptr for child stories because there are no cyclic links.
            std::vector<std::shared_ptr<Category>> mChildCategories;
            std::vector<std::shared_ptr<ScenarioBase>> mChildScenarios;
        };
        
        std::ostream & operator << (std::ostream & strm, const Category & category);
        
        class ScenarioManager
        {
        public:
            virtual ~ScenarioManager ()
            { }
            
            static std::shared_ptr<ScenarioManager> instance ()
            {
                static std::shared_ptr<ScenarioManager> staticInstance(new ScenarioManager());
                
                return staticInstance;
            }
            
            std::vector<std::shared_ptr<Category>> categories ()
            {
                return mTopLevelCategories;
            }
            
            virtual std::shared_ptr<Category> registerCategory (const std::string & categoryFullName)
            {
                // Skip over initial forward slash characters.
                std::string::size_type beginPosition = categoryFullName.find_first_not_of("/");
                if (beginPosition == std::string::npos)
                {
                    throw std::invalid_argument("Category names cannot consist entirely of / characters.");
                }
                
                std::string::size_type currentBeginPosition = beginPosition;
                std::string::size_type currentEndPosition;
                std::shared_ptr<Category> previousCategory;
                while (currentBeginPosition != std::string::npos)
                {
                    currentEndPosition = categoryFullName.find_first_of("/", currentBeginPosition);
                    if (currentEndPosition == std::string::npos)
                    {
                        currentEndPosition = categoryFullName.length();
                    }
                    
                    std::string currentFullName = categoryFullName.substr(beginPosition,
                        currentEndPosition - beginPosition);
                    std::string currentName = categoryFullName.substr(currentBeginPosition,
                        currentEndPosition - currentBeginPosition);
                    
                    if (currentName.empty())
                    {
                        throw std::invalid_argument("Category names cannot be empty.");
                    }
                    
                    auto categoryIter = mAllCategories.find(currentFullName);
                    if (categoryIter == mAllCategories.end())
                    {
                        std::shared_ptr<Category> newCategory(new Category(currentName));
                        categoryIter = mAllCategories.insert({currentFullName, newCategory}).first;
                        if (previousCategory)
                        {
                            previousCategory->mChildCategories.push_back(newCategory);
                        }
                        else
                        {
                            mTopLevelCategories.push_back(newCategory);
                        }
                    }
                    
                    previousCategory = categoryIter->second;
                    
                    currentBeginPosition = categoryFullName.find_first_not_of("/", currentEndPosition);
                }
                
                return previousCategory;
            }
            
            virtual void run (std::ostream & stream)
            {
                for (auto & category : mTopLevelCategories)
                {
                    category->run(stream);
                }
            }
            
        private:
            ScenarioManager ()
            {
                mAllCategories.clear();
                mTopLevelCategories.clear();
            }
            
            std::map<std::string, std::shared_ptr<Category>> mAllCategories;
            std::vector<std::shared_ptr<Category>> mTopLevelCategories;
        };
        
    } // namespace Designer
    
} // namespace MuddledManaged

#define INTERNAL_DESIGNER_SCENARIO_CLASS_NAME_FINAL( name, line ) ClassDesigner ## name ## line
#define INTERNAL_DESIGNER_SCENARIO_CLASS_NAME_RELAY( name, line ) INTERNAL_DESIGNER_SCENARIO_CLASS_NAME_FINAL( name, line )
#define INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( name ) INTERNAL_DESIGNER_SCENARIO_CLASS_NAME_RELAY( name, __LINE__ )

#define INTERNAL_DESIGNER_SCENARIO_INSTANCE_NAME_FINAL( name, line ) InstanceDesigner ## name ## line
#define INTERNAL_DESIGNER_SCENARIO_INSTANCE_NAME_RELAY( name, line ) INTERNAL_DESIGNER_SCENARIO_INSTANCE_NAME_FINAL( name, line )
#define INTERNAL_DESIGNER_SCENARIO_INSTANCE_NAME( name ) INTERNAL_DESIGNER_SCENARIO_INSTANCE_NAME_RELAY( name, __LINE__ )

#define DESIGNER_SCENARIO( preprocGroupName, preprocCategoryName, preprocScenarioDescription ) class INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( preprocGroupName ) \
: public Designer::Scenario<> \
{ \
public: \
    INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( preprocGroupName ) (const std::string & categoryFullName, const std::string & scenarioDescription, bool exceptionExpected) \
    : Designer::Scenario<>(categoryFullName, scenarioDescription, exceptionExpected) \
    { \
        auto scenarioManager = Designer::ScenarioManager::instance(); \
        auto category = scenarioManager->registerCategory(categoryFullName); \
        category->registerScenario(this); \
    } \
    virtual std::shared_ptr<Designer::ScenarioBase> clone () const \
    { \
        return std::shared_ptr<Designer::ScenarioBase>(new INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( preprocGroupName )(*this)); \
    } \
    virtual void runSteps (); \
protected: \
    INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( preprocGroupName ) (const INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( preprocGroupName ) & src) \
    : Designer::Scenario<>(src) \
    { } \
}; \
INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( preprocGroupName ) INTERNAL_DESIGNER_SCENARIO_INSTANCE_NAME( preprocGroupName )(preprocCategoryName, preprocScenarioDescription, false); \
void INTERNAL_DESIGNER_SCENARIO_CLASS_NAME( preprocGroupName )::runSteps ()

#ifdef DESIGNER_GENERATE_MAIN

namespace MuddledManaged
{
    namespace Designer
    {
        int main (int argc, const char * argv[])
        {
            auto scenarioManager = ScenarioManager::instance();
            
            scenarioManager->run(std::cout);
            
            return 0;
        }
        
    } // namespace Designer
    
} // namespace MuddledManaged

using namespace MuddledManaged;

int main (int argc, const char * argv[])
{
    int result = Designer::main(argc, argv);
    
    std::cout << "Completed all scenarios.\n";
    return result;
}

#endif // DESIGNER_GENERATE_MAIN

#ifdef DESIGNER_GENERATE_GLOBALS

namespace MuddledManaged
{
    namespace Designer
    {
        std::ostream & operator << (std::ostream & strm, const Category & category)
        {
            return strm;
        }
        
    } // namespace Designer
    
} // namespace MuddledManaged

#endif // DESIGNER_GENERATE_GLOBALS

#endif // Designer_Designer_h
