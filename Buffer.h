class   Buffer
{
  int			size_;
  Test**		array_;
  int   		inIndex_;
  int   		outIndex_;
  int   		numItems_;
  pthread_mutex_t lock_;
  pthread_cond_t  notFull_;
  pthread_cond_t  notEmpty_;

public :

  Buffer		(int	size	= 1
  			) :
			size_(size)
  {
    pthread_mutex_init(&lock_,NULL);
    pthread_cond_init(&notFull_,NULL);
    pthread_cond_init(&notEmpty_,NULL);
    array_	= (Test**)calloc(size,sizeof(Test*));
    inIndex_ = outIndex_ = numItems_ = 0;
  }

  ~Buffer       ()
  {
    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&notFull_);
    pthread_cond_destroy(&notEmpty_);
    free(array_);
  }

  int   getNumItems  () const
  { return(numItems_); }

  int	getSize	       () const
  { return(size_); }

  void  putIn (Test*	testPtr)
  {
    pthread_mutex_lock(&lock_);
    while  (getNumItems() >= getSize())
    {
      // printf("Full!  Waiting!\n");
      pthread_cond_wait(&notFull_,&lock_);
    }

    array_[inIndex_] = testPtr;

    inIndex_++;
    numItems_++;
    if  (inIndex_ >= getSize())
      inIndex_ = 0;
    pthread_cond_signal(&notEmpty_);
    pthread_mutex_unlock(&lock_);
  }

  Test*		pullOut		()
  {
    pthread_mutex_lock(&lock_);
    while  (getNumItems() <= 0)
    {
      // printf("Empty!  Waiting!\n");
      pthread_cond_wait(&notEmpty_,&lock_);
    }

    Test* toReturn        = array_[outIndex_];
    
    outIndex_++;
    numItems_--;
    if  (outIndex_ >= getSize())
      outIndex_ = 0;
    pthread_cond_signal(&notFull_);
    pthread_mutex_unlock(&lock_);
    return(toReturn);
  }
};